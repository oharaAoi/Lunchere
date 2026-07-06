#include "BossActionVerticalMissile.h"
#include "Game/Actor/Boss/Boss.h"
#include "Engine/Lib/Json/JsonItems.h"
#include "Game/Actor/Boss/Bullet/BossMissile.h"
#include "Game/UI/Boss/BossUIs.h"

namespace {
constexpr float kShotInterval = 0.2f;
constexpr float kLaneOffsets[] = { 2.0f, -2.0f };
constexpr float kBaseShotAngleDeg = 90.0f;
constexpr float kShotAngleStepDeg = 10.0f;
constexpr float kMissileDamage = 30.0f;
}

BehaviorStatus BossActionVerticalMissile::Execute() {
	return Action();
}

float BossActionVerticalMissile::EvaluateWeight() {
	float result = weight_ * CalcAggressionScore(pTarget_->GetAggressionScore());
	return std::clamp(result, 0.0f, 1.0f);
}
///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossActionVerticalMissile::Debug_Gui() {
	BaseTaskNode::Debug_Gui();
	param_.Debug_Gui();
}

void BossActionVerticalMissile::Parameter::Debug_Gui() {
	ImGui::DragFloat("bulletSpeed", &bulletSpeed);
	ImGui::DragFloat("fireRadius", &fireRadius);
	ImGui::DragInt("kFireCount", &kFireCount);
	SaveAndLoad();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 終了確認
///////////////////////////////////////////////////////////////////////////////////////////////

bool BossActionVerticalMissile::IsFinish() {
	if (param_.kFireCount <= fireCount_) {
		return true;
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 実行可能確認
//////////////////////////////////////////////////////////////////////////////////////////////

bool BossActionVerticalMissile::CanExecute() {
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossActionVerticalMissile::Init() {
	param_.Load();

	taskTimer_ = 0.0f;
	fireCount_ = 0;

	attackStart_ = false;

	pTarget_->GetUIs()->PopAlert(pTarget_->GetTargetPos(), pTarget_->GetPosition());
	pTarget_->SetIsAttack(false);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossActionVerticalMissile::Update() {
	if (!attackStart_) {
		attackStart_ = pTarget_->TargetLook();
		return;
	}

	taskTimer_ += AOENGINE::GameTimer::DeltaTime();
	if (taskTimer_ > kShotInterval) {
		Shot();
		taskTimer_ = 0.0f;
	} 
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 終了処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossActionVerticalMissile::End() {
	pTarget_->SetIsAttack(true);
}

void BossActionVerticalMissile::Shot() {
	Math::Vector3 forward = pTarget_->GetTransform()->GetRotate().MakeForward();
	Math::Vector3 right = pTarget_->GetTransform()->GetRotate().MakeRight();

	angle_ = kBaseShotAngleDeg - (static_cast<float>(fireCount_) * kShotAngleStepDeg);
	angle_ *= -kToRadian;
	// クォータニオンで回転を生成（up軸周りに角度回転）
	Math::Quaternion rot = Math::Quaternion::AngleAxis(angle_, right);
	
	// 正面ベクトルを回転させて発射方向に
	Math::Vector3 dir = rot.Rotate(forward);
	
	for (float laneOffset : kLaneOffsets) {
		Math::Vector3 pos = pTarget_->GetTransform()->GetWorldPos() + (param_.fireRadius * dir);
		pos += right * laneOffset;
		Math::Vector3 velocity = dir.Normalize() * param_.bulletSpeed;
		BossMissile* missile = pTarget_->GetBulletManager()->AddBullet<BossMissile>(pos, velocity, pTarget_->GetTargetPos(),
																					param_.bulletSpeed, param_.firstSpeedRaito, param_.trakingRaito, true);
		missile->SetTakeDamage(kMissileDamage);
	}

	fireCount_++;
}
