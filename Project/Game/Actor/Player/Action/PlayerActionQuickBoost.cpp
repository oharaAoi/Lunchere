#include "PlayerActionQuickBoost.h"
#include "Game/Actor/Player/Player.h"
#include "Game/Actor/Player/Action/PlayerActionMove.h"
#include "Game/Actor/Player/Action/PlayerActionBoost.h"
#include "Engine/System/Manager/ParticleManager.h"
#include "Engine/Lib/GameTimer.h"
#include "Engine/Core/Engine.h"
#include <Module/PostEffect/PostProcess.h>

namespace {
constexpr float kBoostInputDeadZone = 0.1f;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集処理
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerActionQuickBoost::Debug_Gui() {
	ImGui::Text("boostForce: (%.2f)", param_.boostForce);
	ImGui::Text("boostEnergy: (%.2f)", param_.boostEnergy);
	float t = actionTimer_ / param_.decelerationTime;
	float bezierValue = param_.decelerationCurve.BezierValue(1 - t);
	ImGui::Text("bezierValue: (%.2f)", bezierValue);
	initParam_.Debug_Gui();
}

void PlayerActionQuickBoost::Parameter::Debug_Gui() {
	ImGui::DragFloat("boostForce", &boostForce, 0.1f);
	ImGui::DragFloat("boostEnergy", &boostEnergy, 0.01f);
	ImGui::DragFloat("cameraShakeTime", &cameraShakeTime, 0.1f);
	ImGui::DragFloat("cameraShakeStrength", &cameraShakeStrength, 0.1f);
	ImGui::DragFloat("decelerationTime", &decelerationTime, 0.1f);
	decelerationCurve.Debug_Gui();
	SaveAndLoad();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 設定時のみ行う処理
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerActionQuickBoost::Build() {
	SetName("actionQuickBoost");
	pInput_ = AOENGINE::Input::GetInstance();
	pOwnerTransform_ = pOwner_->GetTransform();

	initParam_.SetGroupName(pManager_->GetName());
	initParam_.Load();

	pRigidBody_ = pOwner_->GetGameObject()->GetRigidbody();

	pRadialBlur_ = Engine::GetPostProcess()->GetEffectAs<PostEffect::RadialBlur>(PostEffectType::RadialBlur);

	boostClassId_ = typeid(PlayerActionBoost).hash_code();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerActionQuickBoost::OnStart() {
	param_ = initParam_;

	// 初速度を求める
	stick_ = pInput_->GetLeftJoyStick().Normalize();

	if (pManager_->ExistAction(boostClassId_)) {
		Math::Vector3 right = pOwnerTransform_->GetRotate().MakeRight()* stick_.x;
		direction_ = right;
	} else {
		if (stick_.Length() <= kBoostInputDeadZone) {
			direction_ = pOwner_->GetTransform()->GetRotate().MakeForward();
		} else {
			direction_ = pOwner_->GetFollowCamera()->GetAngleX().Rotate(Math::Vector3{ stick_.x, 0.0f, stick_.y });
		}

		// ブラーを画面にかける
		pRadialBlur_->Start(param_.blurStrength, param_.blurTime, false);
	}

	acceleration_ = direction_ * param_.boostForce;
	pRigidBody_->SetVelocity(acceleration_);

	// エネルギーを消費する
 	PlayerParameter& ownerParam_ = pOwner_->GetParam();
	ownerParam_.energy -= param_.boostEnergy;

	actionTimer_ = 0;

	pManager_->DeleteAction(typeid(PlayerActionMove).hash_code());
	pInput_->Vibrate(param_.vibrateStrength, param_.vibrateTime);

	// カメラを揺らす
	pOwner_->GetFollowCamera()->SetShake(param_.cameraShakeTime, param_.cameraShakeStrength);
	// boostをonにする
	pOwner_->GetJetEngine()->BoostOn();
	pOwner_->GetJetEngine()->JetIsStart();


	pOwner_->GetJetEngine()->StartQuickBoost();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerActionQuickBoost::OnUpdate() {
	actionTimer_ += AOENGINE::GameTimer::DeltaTime();

	Boost();
	pOwner_->UpdateJoint();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 終了
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerActionQuickBoost::OnEnd() {
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 次に行うアクションの判定
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerActionQuickBoost::CheckNextAction() {
	if (actionTimer_ >= param_.decelerationTime) {
		// boost中であるならば自身を削除
		if (pManager_->ExistAction(boostClassId_)) {
			DeleteSelf();
		} else {
			NextAction<PlayerActionMove>();
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 入力処理
///////////////////////////////////////////////////////////////////////////////////////////////

bool PlayerActionQuickBoost::IsInput() {
	if (pInput_->IsTriggerButton(XInputButtons::ButtonX)) {
		if (pOwner_->GetParam().energy > 0.0f) {
			return true;
		}
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ main action
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerActionQuickBoost::Boost() {
	float t = actionTimer_ / param_.decelerationTime;
	float bezierValue = param_.decelerationCurve.BezierValue(t);

	acceleration_ = direction_ * (param_.boostForce * bezierValue);

	pRigidBody_->AddVelocity(acceleration_ * AOENGINE::GameTimer::DeltaTime());
}
