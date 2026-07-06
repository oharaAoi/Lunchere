#include "PlayerActionMove.h"
#include "Game/Actor/Player/Player.h"
#include "Game/Actor/Player/Action/PlayerActionIdle.h"
#include "Game/Actor/Player/Action/PlayerActionJump.h"
#include "Game/Actor/Player/Action/PlayerActionQuickBoost.h"
#include "Game/Actor/Player/Action/PlayerActionBoost.h"
#include "Game/Actor/Player/Action/PlayerActionShotRight.h"
#include "Game/Actor/Player/Action/PlayerActionShotLeft.h"
#include "Game/Actor/Player/Action/PlayerActionRightShoulder.h"
#include "Game/Actor/Player/Action/PlayerActionLeftShoulder.h"
// Engine
#include "Engine/Lib/GameTimer.h"
#include "Engine/Module/Components/Animation/AnimationClip.h"
#include "Engine/System/Input/Input.h"

namespace {
constexpr float kStopVelocityThreshold = 0.1f;
constexpr float kSideVelocityDamping = 5.0f;
}

void PlayerActionMove::Debug_Gui() {
	const Math::Vector3 velocity = pOwner_->GetGameObject()->GetRigidbody()->GetMoveForce();
	ImGui::Text("accel_ : x(%f) y(%f) z(%f)", accel_.x, accel_.y, accel_.z);
	ImGui::Text("velocity : x(%f) y(%f) z(%f)", velocity_.x, velocity_.y, velocity_.z);
	ImGui::Text("rigidBodyVelocity : x(%f) y(%f) z(%f)", velocity.x, velocity.y, velocity.z);
	param_.Debug_Gui();
	param_.rotateT = std::clamp(param_.rotateT, 0.0f, 1.0f);
}

void PlayerActionMove::Parameter::Debug_Gui() {
	ImGui::DragFloat("speed", &speed, 0.1f);
	ImGui::DragFloat("boostSpeed", &boostSpeed, 0.1f);
	ImGui::DragFloat("maxSpeed", &maxSpeed, 0.1f);
	ImGui::DragFloat("rotateT", &rotateT, 0.01f);
	ImGui::DragFloat("decayRate", &decayRate, 0.1f);
	ImGui::DragFloat("turnAroundThreshold", &turnAroundThreshold, 0.1f);
	ImGui::DragFloat("turnSpeed", &turnSpeed, 0.1f);
	ImGui::DragFloat("animtionTime", &animationTime, 0.1f);
	ImGui::DragFloat("accel", &accel, 0.1f);
	ImGui::DragFloat("decel", &decel, 0.1f);
	SaveAndLoad();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 設定時のみ行う処理
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerActionMove::Build() {
	SetName("actionMove");

	param_.SetGroupName(pManager_->GetName());
	param_.Load();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerActionMove::OnStart() {
	actionTimer_ = 0;
	pRigidbody_ = pOwner_->GetGameObject()->GetRigidbody();

	pOwner_->SetIsMoving(true);
	isTurnAround_ = false;

	inputStick_ = AOENGINE::Input::GetInstance()->GetLeftJoyStick(kDeadZone_).Normalize();
	Math::Vector3 dire = pOwner_->GetFollowCamera()->GetAngleX().Rotate(Math::Vector3{ inputStick_.x, 0.0f, inputStick_.y });
	if (pOwner_->GetIsBoostMode()) {
		accel_ = dire * param_.boostSpeed;
	} else {
		accel_ = dire * param_.speed;
	}
	velocity_ = accel_ + pRigidbody_->GetVelocity();

	// ----------------------
	// ↓ Animationの設定
	// ----------------------
	if (pOwner_->GetIsBoostMode()) {
		AOENGINE::AnimationClip* clip = pOwner_->GetGameObject()->GetAnimator()->GetAnimationClip();
		clip->PoseToAnimation("move", param_.animationTime);
		clip->SetIsLoop(false);
	} else {
		AOENGINE::AnimationClip* clip = pOwner_->GetGameObject()->GetAnimator()->GetAnimationClip();
		clip->PoseToAnimation("walk", param_.animationTime);
		clip->SetIsLoop(true);
	}

	// ----------------------
	// ↓ boostの設定
	// ----------------------
	if (pOwner_->GetIsBoostMode() || !pOwner_->GetIsLanding()) {
		pOwner_->GetJetEngine()->JetIsStart();
	} else {
		pOwner_->GetJetEngine()->JetIsStop();
	}

	pOwner_->UpdateJoint();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新
///////////////////////////////////////////////////////////////////////////////////////////////

void PlayerActionMove::OnUpdate() {
	actionTimer_ += AOENGINE::GameTimer::DeltaTime();

	if (preBoost_ != pOwner_->GetIsBoostMode()) {
		if (preBoost_) {
			AOENGINE::AnimationClip* clip = pOwner_->GetGameObject()->GetAnimator()->GetAnimationClip();
			clip->PoseToAnimation("walk", param_.animationTime);
			clip->SetIsLoop(true);
		} else {
			AOENGINE::AnimationClip* clip = pOwner_->GetGameObject()->GetAnimator()->GetAnimationClip();
			clip->PoseToAnimation("move", param_.animationTime);
			clip->SetIsLoop(false);
		}
	}

	Move();

	// 移動の入力がなかったら回復処理を行っていく
	if (pOwner_->GetIsLanding()) {
		pOwner_->RecoveryEN(actionTimer_);
	}

	preBoost_ = pOwner_->GetIsBoostMode();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 終了
//////////////////////////////////////////////////////////////////////////////////////////////

void PlayerActionMove::OnEnd() {
	pOwner_->SetIsMoving(false);
	pOwner_->GetJetEngine()->JetIsStop();
	if (pOwner_->GetIsBoostMode()) {
		AOENGINE::AnimationClip* clip = pOwner_->GetGameObject()->GetAnimator()->GetAnimationClip();
		clip->PoseToAnimation("move_cancel", param_.animationTime);
		clip->SetIsLoop(false);
	} else {
		AOENGINE::AnimationClip* clip = pOwner_->GetGameObject()->GetAnimator()->GetAnimationClip();
		clip->PoseToAnimation("idle", param_.animationTime);
		clip->SetIsLoop(false);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 次に行うアクションの判定
//////////////////////////////////////////////////////////////////////////////////////////////

void PlayerActionMove::CheckNextAction() {
	if (pRigidbody_->GetVelocity().Length() <= kStopVelocityThreshold) {
		if (pOwner_->GetIsLanding()) {
			NextAction<PlayerActionIdle>();
		}
	}

	if (CheckInput<PlayerActionJump>()) {
		AddAction<PlayerActionJump>();
	}

	if (CheckInput<PlayerActionShotRight>()) {
		AddAction<PlayerActionShotRight>();
	}

	if (CheckInput<PlayerActionShotLeft>()) {
		AddAction<PlayerActionShotLeft>();
	}

	if (CheckInput<PlayerActionRightShoulder>()) {
		AddAction<PlayerActionRightShoulder>();
	}

	if (CheckInput<PlayerActionLeftShoulder>()) {
		AddAction<PlayerActionLeftShoulder>();
	}

	if (CheckInput<PlayerActionQuickBoost>()) {
		NextAction<PlayerActionQuickBoost>();
	}

	if (CheckInput<PlayerActionBoost>()) {
		NextAction<PlayerActionBoost>();
	}

}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 入力処理
///////////////////////////////////////////////////////////////////////////////////////////////

bool PlayerActionMove::IsInput() {
	if (pManager_->ExistAction(typeid(PlayerActionQuickBoost).hash_code())) {
		return false;
	}

	Math::Vector2 current = AOENGINE::Input::GetInstance()->GetLeftJoyStick(kDeadZone_);
	if (current.x != 0.0f || current.y != 0.0f) {
		return true;
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ main action
///////////////////////////////////////////////////////////////////////////////////////////////


void PlayerActionMove::Move() {
	inputStick_ = AOENGINE::Input::GetInstance()->GetLeftJoyStick(kDeadZone_).Normalize();

	// ----------------------
	// 移動スピード設定
	// ----------------------
	float speed = pOwner_->GetIsBoostMode() ? param_.boostSpeed : param_.speed;

	// 入力方向ベクトル
	Math::Vector3 dire = pOwner_->GetFollowCamera()->GetAngleX().Rotate(Math::Vector3{ inputStick_.x, 0.0f, inputStick_.y });
	Math::Vector3 targetVelocity = dire * speed;

	// ----------------------
	// 滑りの実装
	// ----------------------
	if (inputStick_.Length() > kDeadZone_) {
		Math::Vector3 forward = targetVelocity.Normalize();
		float forwardMag = Math::Vector3::Dot(velocity_, forward);
		Math::Vector3 side = velocity_ - forward * forwardMag;

		// 横成分を減衰（重量感に合わせて 5〜10 程度）
		side = Lerp(side, CVector3::ZERO, kSideVelocityDamping * AOENGINE::GameTimer::DeltaTime());
		velocity_ = (forward * forwardMag + side);
	}

	// ----------------------
	// 反転処理
	// ----------------------
	float dot = Math::Vector2::Dot(preInputStick_, inputStick_);
	if (dot < param_.turnAroundThreshold) {
		velocity_ *= param_.turnSpeed * AOENGINE::GameTimer::DeltaTime();
	}

	// ----------------------
	// 加減速制御
	// ----------------------
	if (inputStick_.Length() > kDeadZone_) {
		velocity_ = Lerp(velocity_, targetVelocity, param_.accel * AOENGINE::GameTimer::DeltaTime());
	} else {
		velocity_ = Lerp(velocity_, CVector3::ZERO, param_.decel * AOENGINE::GameTimer::DeltaTime());
	}

	// ----------------------
	// 速度制限
	// ----------------------
	float length = velocity_.Length();
	if (length > param_.maxSpeed) {
		velocity_ = velocity_.Normalize() * param_.maxSpeed * AOENGINE::GameTimer::DeltaTime();
	}

	// Rigidbodyへ適用
	pRigidbody_->AddVelocityX(velocity_.x);
	pRigidbody_->AddVelocityZ(velocity_.z);

	// ----------------------
	// 向き更新
	// ----------------------
	pOwner_->LookTarget(param_.rotateT, pOwner_->GetReticle()->GetLockOn());

	preVelocity_ = velocity_;
	preInputStick_ = inputStick_;
}
