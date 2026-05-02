#include "PulseArmor.h"
#include "Engine/Core/Engine.h"
#include "Engine/Render/Render.h"
#include "Engine/Core/GraphicsContext.h"
#include "Engine/Lib/Math/MyRandom.h"
#include "Engine/Lib/Json/JsonItems.h"
#include "Engine/Lib/GameTimer.h"
#include "Engine/System/Manager/MeshManager.h"
#include "Engine/System/Manager/TextureManager.h"

PulseArmor::~PulseArmor() {
	settingBuffer_->Destroy();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void PulseArmor::Init() {
	armorParam_.SetGroupName(GetName());
	armorParam_.Load();

	geometry_.Init(CMath::Vector2::UNIT, 16, "armor");

	// meshの設定
	std::string name = geometry_.GetGeometryName();
	if (!AOENGINE::MeshManager::GetInstance()->ExistMesh(name)) {
		mesh_ = std::make_shared<AOENGINE::Mesh>();
		mesh_->Init(AOENGINE::GraphicsContext::GetInstance()->GetDevice(), geometry_.GetVertex(), geometry_.GetIndex());
		AOENGINE::MeshManager::GetInstance()->AddMesh(AOENGINE::GraphicsContext::GetInstance()->GetDevice(), name, name, mesh_->GetVerticesData(), mesh_->GetIndices());
	} else {
		mesh_ = AOENGINE::MeshManager::GetInstance()->GetMesh(name);
	}

	// material/worldTransformに関する設定
	material_ = std::make_unique<AOENGINE::Material>();
	material_->Init();
	worldTransform_ = std::make_unique<AOENGINE::WorldTransform>();
	worldTransform_->Init();
	material_->SetAlbedoTexture(armorParam_.baseTexture);

	// dissolvebufferに関する設定
	AOENGINE::GraphicsContext* graphicsCtx = AOENGINE::GraphicsContext::GetInstance();
	settingBuffer_ = graphicsCtx->CreateDxResource(ResourceType::Common);
	settingBuffer_->CreateResource(sizeof(DissolveSetting));
	settingBuffer_->GetResource()->Map(0, nullptr, reinterpret_cast<void**>(&setting_));

	// uvSrtの初期化
	for (size_t index = 0; index < 3; ++index) {
		uvSrt_[index].scale = CVector3::UNIT;
		uvSrt_[index].rotate = CVector3::ZERO;
		uvSrt_[index].translate = CVector3::ZERO;

		setting_->uvTransform[index] = uvSrt_[index].MakeAffine();
	}

	// bufferPtrの初期化
	setting_->color = Colors::Linear::white;
	setting_->edgeColor = Colors::Linear::white;
	setting_->threshold = 0.02f;

	noiseTexture_[0] = "noise0.png";
	noiseTexture_[1] = "noise5.png";
	noiseTexture_[2] = "noise6.png";

	for (size_t index = 0; index < 3; ++index) {
		Math::Vector3 min = Random::RandomVector3(CVector3::UNIT * -5.0f, CVector3::UNIT * 5.0f);
		Math::Vector3 max = Random::RandomVector3(CVector3::UNIT * 5.0f, CVector3::UNIT * 5.0f);
		uvMovingTween_[index].Init(min, max, Random::RandomFloat(100.0f, 200.0f), (int)EasingType::None::Liner, LoopType::Return);
	}

	thresholdTween_.Init(armorParam_.HighDurabilityMinThreshold, armorParam_.HighDurabilityMaxThreshold, armorParam_.duration, (int)EasingType::None::Liner, LoopType::Return);

	isAlive_ = false;
	worldTransform_->SetScale(CVector3::ZERO);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void PulseArmor::Update() {
	// uvの更新を行う
	for (size_t index = 0; index < 3; ++index) {
		uvSrt_[index].translate = uvMovingTween_[index].GetValue();
		uvMovingTween_[index].Update(AOENGINE::GameTimer::DeltaTime());
		setting_->uvTransform[index] = uvSrt_[index].MakeAffine();
	}

	// 閾値のアニメーション
	thresholdTween_.Update(AOENGINE::GameTimer::DeltaTime());
	setting_->threshold = thresholdTween_.GetValue();

	// 座標の更新
	worldTransform_->Update();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 描画処理
///////////////////////////////////////////////////////////////////////////////////////////////

void PulseArmor::Draw() const {
	if (!isAlive_) { return; }

	Engine::SetPipeline(PSOType::Object3d, "Object_Dissolve.json");
	AOENGINE::Pipeline* pso = Engine::GetLastUsedPipeline();
	ID3D12GraphicsCommandList* commandList = AOENGINE::GraphicsContext::GetInstance()->GetCommandList();

	// VS
	commandList->IASetVertexBuffers(0, 1, &mesh_->GetVBV());
	commandList->IASetIndexBuffer(&mesh_->GetIBV());

	UINT index = pso->GetRootSignatureIndex("gWorldTransformMatrix");
	worldTransform_->BindCommandList(commandList, index);
	index = pso->GetRootSignatureIndex("gViewProjectionMatrix");
	AOENGINE::Render::GetInstance()->GetViewProjection()->BindCommandList(commandList, index);
	index = pso->GetRootSignatureIndex("gViewProjectionMatrixPrev");
	AOENGINE::Render::GetInstance()->GetViewProjection()->BindCommandListPrev(commandList, index);

	// PS
	index = pso->GetRootSignatureIndex("gSetting");
	commandList->SetGraphicsRootConstantBufferView(index, settingBuffer_->GetResource()->GetGPUVirtualAddress());
	index = pso->GetRootSignatureIndex("gTexture");
	AOENGINE::TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(commandList, material_->GetAlbedoTexture(), index);
	index = pso->GetRootSignatureIndex("gMaskTexture1");
	AOENGINE::TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(commandList, noiseTexture_[0], index);
	index = pso->GetRootSignatureIndex("gMaskTexture2");
	AOENGINE::TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(commandList, noiseTexture_[1], index);
	index = pso->GetRootSignatureIndex("gMaskTexture3");
	AOENGINE::TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(commandList, noiseTexture_[2], index);

	commandList->DrawIndexedInstanced(mesh_->GetIndexNum(), 1, 0, 0, 0);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集処理
///////////////////////////////////////////////////////////////////////////////////////////////

void PulseArmor::Debug_Gui() {
	ImGui::DragFloat("耐久度", &durability_, 0.1f);
	worldTransform_->Debug_Gui();
	material_->Debug_Gui();

	// dissolveに関する設定を行う
	if (ImGui::CollapsingHeader("Setting")) {
		ImGui::ColorEdit4("color", &setting_->color.r);
		ImGui::ColorEdit4("edgeColor", &setting_->edgeColor.r);
		ImGui::DragFloat("threshold", &setting_->threshold, 0.01f);

		for (size_t index = 0; index < 3; ++index) {
			std::string name = "Noise" + std::to_string(index);
			if (ImGui::TreeNode(name.c_str())) {
				noiseTexture_[index] = AOENGINE::TextureManager::GetInstance()->SelectTexture(noiseTexture_[index]);
				ImGui::TreePop();
			}
		}

		setting_->threshold = std::clamp(setting_->threshold, 0.0f, 1.0f);
	}

	// アーマーのパラメータを編集する
	if (ImGui::CollapsingHeader("Parameter")) {
		ImGui::DragFloat("耐久度", &armorParam_.durability, 0.1f);
		ImGui::DragFloat3("大きさ", &armorParam_.scale.x, 0.1f);

		ImGui::DragFloat("最小値 : 耐久が低い時", &armorParam_.lowDurabilityMinThreshold, 0.01f, 0.f, 1.f);
		ImGui::DragFloat("最小値 : 耐久が高い時", &armorParam_.HighDurabilityMinThreshold, 0.01f, 0.f, 1.f);
		ImGui::DragFloat("最大値 : 耐久が低い時", &armorParam_.lowDurabilityMaxThreshold, 0.01f, 0.f, 1.f);
		ImGui::DragFloat("最大値 : 耐久が高い時", &armorParam_.HighDurabilityMaxThreshold, 0.01f, 0.f, 1.f);
		ImGui::DragFloat("グラデーションの時間", &armorParam_.duration, 0.01f);
		
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen;
		if (ImGui::TreeNodeEx("uvTransform", flags)) {
			if (ImGui::TreeNode("scale")) {
				ImGui::DragFloat3("uvScale", &armorParam_.uvTransform.scale.x, 0.01f);
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("rotation")) {
				ImGui::DragFloat3("uvRotation", &armorParam_.uvTransform.rotate.x, 0.01f);
				ImGui::TreePop();
			}
			ImGui::TreePop();
		}

		if (ImGui::Button("Save")) {
			armorParam_.color = setting_->color;
			armorParam_.edgeColor = setting_->edgeColor;
			armorParam_.baseTexture = material_->GetAlbedoTexture();
			armorParam_.noiseTexture1 = noiseTexture_[0];
			armorParam_.noiseTexture2 = noiseTexture_[1];
			armorParam_.noiseTexture3 = noiseTexture_[2];
			armorParam_.Save();
		}

		if (ImGui::Button("Applay")) {
			armorParam_.scale = worldTransform_->GetScale();
			armorParam_.localTranslate = worldTransform_->GetWorldPos();
			armorParam_.color = setting_->color;
			armorParam_.edgeColor = setting_->edgeColor;
			armorParam_.uvTransform.scale = uvSrt_[0].scale;
			armorParam_.uvTransform.rotate = uvSrt_[0].rotate;
			thresholdTween_.Init(armorParam_.HighDurabilityMinThreshold, armorParam_.HighDurabilityMaxThreshold, Random::RandomFloat(4.0f, 8.0f), (int)EasingType::None::Liner, LoopType::Return);
			SetParameter();
		}
	}
}

void PulseArmor::ArmorParameter::Debug_Gui() {

}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ パラメータの設定
///////////////////////////////////////////////////////////////////////////////////////////////

void PulseArmor::SetParameter() {
	SetArmor(armorParam_.durability, armorParam_.scale, armorParam_.color, armorParam_.edgeColor, armorParam_.uvTransform);

	noiseTexture_[0] = armorParam_.noiseTexture1;
	noiseTexture_[1] = armorParam_.noiseTexture2;
	noiseTexture_[2] = armorParam_.noiseTexture3;

	material_->SetAlbedoTexture(armorParam_.baseTexture);
}

void PulseArmor::SetArmor(float _durability, const Math::Vector3& _scale, const AOENGINE::Color& _color, const AOENGINE::Color& _edgeColor, const Math::SRT& _uvSrt) {
	durability_ = _durability;
	initDurability_ = _durability;
	worldTransform_->SetScale(_scale);
	worldTransform_->SetTranslate(armorParam_.localTranslate);
	setting_->color = _color;
	setting_->edgeColor = _edgeColor;

	for (size_t index = 0; index < 3; ++index) {
		uvSrt_[index].scale = _uvSrt.scale;
		uvSrt_[index].rotate = _uvSrt.rotate;
		uvSrt_[index].translate = _uvSrt.translate;
	}

	isAlive_ = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ ダメージ処理
///////////////////////////////////////////////////////////////////////////////////////////////

void PulseArmor::DamageDurability(float _damage) {
	durability_ -= _damage;

	// 現在の耐久度と元の耐久度から減り具合を求める
	float durabilityRaito = durability_ / armorParam_.durability;
	float minThreshold = std::lerp(armorParam_.lowDurabilityMinThreshold, armorParam_.HighDurabilityMinThreshold, durabilityRaito);
	float maxThreshold = std::lerp(armorParam_.lowDurabilityMaxThreshold, armorParam_.HighDurabilityMaxThreshold, durabilityRaito);
	thresholdTween_.SetStart(minThreshold);
	thresholdTween_.SetEnd(maxThreshold);

	thresholdTween_.SetCurrentTime(0);

}

bool PulseArmor::BreakArmor() {
	if (durability_ <= 0.0f) {
		isAlive_ = false;
		return true;
	}
	return false;
}
