#include "JetEngine.h"
#include "Engine/Render/SceneRenderer.h"
#include "Engine/System/Manager/ParticleManager.h"
#include "Engine/Lib/GameTimer.h"
#include "Engine/Lib/Math/Easing.h"

JetEngine::~JetEngine() {
	Finalize();
}

void JetEngine::Finalize() {
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集処理
///////////////////////////////////////////////////////////////////////////////////////////////

void JetEngine::Debug_Gui() {
	if (ImGui::TreeNode("Engine")) {
		object_->Debug_Gui();
		ImGui::TreePop();
	}

	for (size_t i = 0; i < 2; ++i) {
		std::string name = "Burn" + std::to_string(i);
		if (ImGui::TreeNode(name.c_str())) {
			jetEngineBurn_[i]->Debug_Gui();
			ImGui::TreePop();
		}
	}

	if (ImGui::CollapsingHeader("burnParent")) {
		burnParentTransform_->Debug_Gui();
	}

	param_.Debug_Gui();

	if (ImGui::Button("Stop")) {
		this->JetIsStop();
	}
	ImGui::SameLine();
	if (ImGui::Button("Start")) {
		this->JetIsStart();
	}

	if (ImGui::Button("QuickBoost")) {
		turnBack_ = false;
		isQuickBoost_ = true;
		burnQuickBoostTimer_.timer_ = 0.0f;
		burnQuickBoostTimer_.targetTime_ = param_.burnQuickBoostTime;
	}
}


void JetEngine::Parameter::Debug_Gui() {
	ImGui::DragFloat("エンジンの傾き", &engineIncline);
	ImGui::DragFloat("スケールを大きくする時間", &burnScaleUpTime);
	ImGui::DragFloat("クイックブーストで大きくなる時間", &burnQuickBoostTime);
	ImGui::DragFloat3("ブースト時のスケール", &burnBoostScale.x);
	ImGui::DragFloat3("移動時のスケール", &burnMoveScale.x);
	ImGui::BulletText("BoostUp");
	Math::SelectEasing(quickBoostEaseIndex, "BoostUp");
	ImGui::BulletText("BoostDown");
	Math::SelectEasing(quickBoostDownEaseIndex, "BoostDown");
	burnMoveScaleCurve.Debug_Gui();
	SaveAndLoad();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void JetEngine::Init() {
	SetName("JetEngine");
	param_.Load();

	// -------------------------------------
	// 基本の初期化
	// -------------------------------------

	object_ = AOENGINE::SceneRenderer::GetInstance()->AddObject<AOENGINE::BaseGameObject>(GetName(), "Object_PBR.json");
	object_->SetObject("jet.obj", MaterialType::PBR);
	
	transform_ = object_->GetTransform();
	Math::QuaternionSRT srt = transform_->GetSRT();
	srt.translate = { 0.0f, 2.7f, -0.5f };
	srt.rotate = Math::Quaternion::AngleAxis(30.0f * kToRadian, CVector3::RIGHT);
	transform_->SetSRT(srt);

	// -------------------------------------
	// effectの親にする空のTransform作成
	// -------------------------------------

	burnParentTransform_ = std::make_unique<AOENGINE::WorldTransform>();
	burnParentTransform_->Init();
	burnParentTransform_->SetParent(transform_->GetWorldMatrix());

	// -------------------------------------
	// effectの設定
	// -------------------------------------

	for (size_t i = 0; i < 2; ++i) {
		jetEngineBurn_[i] = std::make_unique<JetEngineBurn>();
		if (i == 0) {
			jetEngineBurn_[i]->Init("jetBurnParameter");
		} else {
			jetEngineBurn_[i]->Init("jetBurnCenterParameter");
		}

		jetEngineBurn_[i]->GetTransform()->SetParent(burnParentTransform_->GetWorldMatrix());

		object_->AddChild(jetEngineBurn_[i].get());
	}

	burnParticle_ = AOENGINE::ParticleManager::GetInstance()->CreateParticle("BurnParticle");
	burnParticle_->SetParent(transform_);
	burnParticle_->SetIsStop(true); 

	// -------------------------------------
	// その他初期化
	// -------------------------------------
	burnScaleUpTimer_.targetTime_ = param_.burnScaleUpTime;
	isBoostMode_ = true;
	isStop_ = true;

	turnBack_ = false;
	isQuickBoost_ = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void JetEngine::Update(float diftX) {
	// 炎が大きくなる時間がまだ過ぎていないならその処理を行なう
	if (burnScaleUpTimer_.Run(AOENGINE::GameTimer::DeltaTime())) {
		float value = param_.burnMoveScaleCurve.BezierValue(burnScaleUpTimer_.t_);

		Math::Vector3 scale;
		if (isStop_) {
			scale = Math::Vector3::Lerp(param_.burnMoveScale, CVector3::ZERO, value);
		} else {
			scale = Math::Vector3::Lerp(CVector3::ZERO, param_.burnMoveScale, value);
		}
		burnParentTransform_->SetScale(scale);
	}

	if (isQuickBoost_) {
		QuickBoost();
	}

	Math::Quaternion engineRotate = Math::Quaternion::AngleAxis(diftX * param_.engineIncline, CVector3::FORWARD);
	Math::Quaternion rotate = Math::Quaternion::Slerp(transform_->GetRotate(), engineRotate, 0.5f);
	transform_->SetRotate(rotate);
	transform_->Update();
	burnParentTransform_->Update();

	for (size_t i = 0; i < 2; ++i) {
		jetEngineBurn_[i]->Update();
	}
}

void JetEngine::PostUpdate() {
	transform_->Update();
	burnParentTransform_->Update();
	for (size_t i = 0; i < 2; ++i) {
		jetEngineBurn_[i]->Update();
	}
}

void JetEngine::JetIsStop() {
	isStop_ = true;
	burnScaleUpTimer_.timer_ = 0;
	burnParticle_->SetIsStop(true);
}

void JetEngine::JetIsStart() {
	isStop_ = false;
	burnScaleUpTimer_.timer_ = 0;
	burnParticle_->SetIsStop(false);
}

void JetEngine::StartQuickBoost() {
	turnBack_ = false;
	isQuickBoost_ = true;
	burnQuickBoostTimer_.timer_ = 0.0f;
	burnQuickBoostTimer_.targetTime_ = param_.burnQuickBoostTime;
}

void JetEngine::QuickBoost() {
	if (burnQuickBoostTimer_.Run(AOENGINE::GameTimer::DeltaTime())) {
		if (!turnBack_) {
			Math::Vector3 scale = Math::Vector3::Lerp(param_.burnMoveScale, 
													  param_.burnBoostScale,
													  Math::CallEasing(param_.quickBoostEaseIndex, burnQuickBoostTimer_.t_));
			burnParentTransform_->SetScale(scale);
		} else {
			Math::Vector3 scale = Math::Vector3::Lerp(param_.burnBoostScale,
													  param_.burnMoveScale,
													  Math::CallEasing(param_.quickBoostDownEaseIndex, burnQuickBoostTimer_.t_));
			burnParentTransform_->SetScale(scale);
		}
	} else if(isQuickBoost_) {

		if (turnBack_) {
			isQuickBoost_ = false;
		}

		turnBack_ = true;
		burnQuickBoostTimer_.timer_ = 0.0f;
	}
}
