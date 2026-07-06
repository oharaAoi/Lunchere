#include "Player.h"
#include <vector>
#include "Engine/System/Editor/Window/EditorWindows.h"
#include "Engine/System/Scene/SceneLoader.h"
#include "Engine/Render/SceneRenderer.h"
#include "Engine/Module/Components/Animation/AnimationClip.h"
#include "Engine/Lib/GameTimer.h"
#include "Game/Actor/Player/State/PlayerIdleState.h"
#include "Game/Actor/Player/State/PlayerKnockbackState.h"
#include "Game/Actor/Player/State/PlayerDeadState.h"
#include "Game/Actor/Player/Action/PlayerActionIdle.h"
#include "Game/Actor/Player/Action/PlayerActionMove.h"
#include "Game/Actor/Player/Action/PlayerActionJump.h"
#include "Game/Actor/Player/Action/PlayerActionQuickBoost.h"
#include "Game/Actor/Player/Action/PlayerActionBoost.h"
#include "Game/Actor/Player/Action/PlayerActionShotRight.h"
#include "Game/Actor/Player/Action/PlayerActionShotLeft.h"
#include "Game/Actor/Player/Action/PlayerActionRightShoulder.h"
#include "Game/Actor/Player/Action/PlayerActionLeftShoulder.h"
#include "Game/Actor/Player/Action/PlayerActionDamaged.h"
#include "Game/Actor/Player/Action/PlayerActionTurnAround.h"
#include "Game/Actor/Player/Action/PlayerActionDeployArmor.h"
#include <Module/PostEffect/PostProcess.h>

namespace {
constexpr float kLockOnRotateRate = 0.9f;
constexpr float kPostureDamageRate = 0.5f;
constexpr float kDamageShakeTime = 0.5f;
constexpr float kDamageShakeStrength = 3.0f;
}

Player::Player() {}
Player::~Player() {
	Finalize();
}

void Player::Finalize() {
	vignette_->SetIsEnable(false);
	jet_ = nullptr;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集処理
///////////////////////////////////////////////////////////////////////////////////////////////

void Player::Debug_Gui() {
	object_->Debug_Gui();

	if (ImGui::CollapsingHeader("ActionManager")) {
		actionManager_->Debug_Gui();
	}

	if (ImGui::CollapsingHeader("現在のパラメータ")) {
		param_.Debug_Gui();
	}

	if (ImGui::CollapsingHeader("初期値のパラメータ(保存はこちら)")) {
		initParam_.Debug_Gui();
	}

	object_->GetRigidbody()->SetDrag(param_.windDrag);

	param_.bodyWeight = std::clamp(param_.bodyWeight, 1.0f, 100.0f);
	invincibleTimer_.targetTime_ = param_.invincibleTime;
	psRecoveryTimer_.targetTime_ = param_.psRecoveryTime;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化
///////////////////////////////////////////////////////////////////////////////////////////////

void Player::Init() {
	SetName("Player");
	initParam_.Load();
	param_ = initParam_;

	AOENGINE::SceneLoader::Objects object = AOENGINE::SceneLoader::GetInstance()->GetObjects("Player");

	// -------------------------------------------------
	// ↓ Object初期化
	// -------------------------------------------------

	InitObject();

	// -------------------------------------------------
	// ↓ Animationの設定
	// -------------------------------------------------
	object_->SetAnimator("./Project/Packages/Game/Load/Models/Player/", "player.gltf", true, true, false);
	object_->GetAnimator()->GetAnimationClip()->PoseToAnimation("idle", 0.0f);
	object_->GetAnimator()->GetAnimationClip()->SetIsLoop(false);

	// -------------------------------------------------
	// ↓ Jet関連
	// -------------------------------------------------

	jet_ = std::make_unique<JetEngine>();
	jet_->Init();
	jet_->SetParent(this);
	AddChild(jet_.get());

	// -------------------------------------------------
	// ↓ Collider関連
	// -------------------------------------------------

	InitCollider();

	// -------------------------------------------------
	// ↓ State関連
	// -------------------------------------------------

	stateMachine_ = std::make_unique<StateMachine<Player>>();
	stateMachine_->Init(this);
	stateMachine_->ChangeState<PlayerIdleState>();

	// -------------------------------------------------
	// ↓ Action関連
	// -------------------------------------------------

	InitAction();

	// -------------------------------------------------
	// ↓ Parameter関連
	// -------------------------------------------------
	isKnockback_ = false;
	isLanding_ = false;
	isMoving_ = false;
	deployArmor_ = false;
	isDead_ = false;
	isAttack_ = false;
	isExplode_ = false;

	param_.postureStability -= initParam_.postureStability;

	AOENGINE::Skeleton* skeleton = object_->GetAnimator()->GetSkeleton();
	weaponBornMatrix_[PlayerWeapon::Left_Weapon] = skeleton->GetSkeletonSpaceMat("left_hand") * transform_->GetWorldMatrix();
	weaponBornMatrix_[PlayerWeapon::Right_Weapon] = skeleton->GetSkeletonSpaceMat("right_hand") * transform_->GetWorldMatrix();
	weaponBornMatrix_[PlayerWeapon::Left_Shoulder] = skeleton->GetSkeletonSpaceMat("left_shoulder") * transform_->GetWorldMatrix();
	weaponBornMatrix_[PlayerWeapon::Right_Shoulder] = skeleton->GetSkeletonSpaceMat("right_shoulder") * transform_->GetWorldMatrix();

	vignetteTween_.Init(0.0f, param_.pinchVignettePower, 1.0f, 1, LoopType::Return);
	vignette_ = Engine::GetPostProcess()->GetEffectAs<PostEffect::Vignette>(PostEffectType::Vignette);

	invincibleTimer_.targetTime_ = param_.invincibleTime;
	psRecoveryTimer_.targetTime_ = param_.psRecoveryTime;

#ifdef _DEBUG
	AOENGINE::EditorWindows::AddObjectWindow(this, GetName());
#endif // _DEBUG

}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新
///////////////////////////////////////////////////////////////////////////////////////////////

void Player::Update() {
	// 姿勢安定回復
	PostureStabilityRecovery();
	// boostの判定
	IsBoostMode();
	// actionの更新
	actionManager_->Update();
	// stateの更新
	stateMachine_->Update();

	// ビネットの更新
	if (param_.health <= initParam_.health * param_.pinchOfPercentage) {
		vignetteTween_.Update(AOENGINE::GameTimer::DeltaTime());
		vignette_->SetPower(vignetteTween_.GetValue());
	}

	// 無敵時間の更新
	if (invincibleTimer_.Run(AOENGINE::GameTimer::DeltaTime())) {
		isInvincible_ = true;
	} else {
		isInvincible_ = false;
	}
}

void Player::PosUpdate() {
	// カメラを傾ける
	CameraIncline();
	jet_->Update(smoothedDiffX_);

	// 攻撃を行う
	isAttack_ = false;
	while (!attackHistory_.empty()) {
		bool isCurrentFrameAttack = false;
		auto [weapon, context] = attackHistory_.front();
		isCurrentFrameAttack = GetWeapon(weapon)->Attack(context);  // 使う
		attackHistory_.pop_front();    // 先頭を削除

		if (isCurrentFrameAttack) {
			isAttack_ = true;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 武器をセットする
///////////////////////////////////////////////////////////////////////////////////////////////

void Player::SetWeapon(BaseWeapon* _weapon, PlayerWeapon type) {
	if (type == PlayerWeapon::Left_Weapon) {
		pWeapons_[Left_Weapon] = _weapon;

	} else if (type == PlayerWeapon::Right_Weapon) {
		pWeapons_[Right_Weapon] = _weapon;

	} else if (type == PlayerWeapon::Left_Shoulder) {
		pWeapons_[PlayerWeapon::Left_Shoulder] = _weapon;

	} else if (type == PlayerWeapon::Right_Shoulder) {
		pWeapons_[PlayerWeapon::Right_Shoulder] = _weapon;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ targetの方向をみる
///////////////////////////////////////////////////////////////////////////////////////////////

void Player::LookTarget(float _rotateT_, bool isLockOn) {
	if (isLockOn) {
		Math::Vector3 toTarget = reticle_->GetTargetPos() - transform_->GetTranslate();
		toTarget.y = 0.0f; // Y軸の高さ成分を無視して水平方向ベクトルに
		toTarget = toTarget.Normalize();
		Math::Quaternion targetToRotate = Math::Quaternion::LookRotation(toTarget);
		transform_->SetRotate(Math::Quaternion::Slerp(transform_->GetRotate(), targetToRotate, kLockOnRotateRate));
	} else {
		transform_->SetRotate(object_->GetRigidbody()->LookVelocity(transform_->GetRotate(), _rotateT_));
	}
}

void Player::Attack(PlayerWeapon _weapon, AttackContext _contex) {
	attackHistory_.emplace_back(std::make_pair(_weapon, _contex));
}

void Player::UpdateJoint() {
	AOENGINE::Skeleton* skeleton = object_->GetAnimator()->GetSkeleton();
	weaponBornMatrix_[PlayerWeapon::Left_Weapon] = skeleton->GetSkeletonSpaceMat("left_hand") * transform_->GetWorldMatrix();
	weaponBornMatrix_[PlayerWeapon::Right_Weapon] = skeleton->GetSkeletonSpaceMat("right_hand") * transform_->GetWorldMatrix();
	weaponBornMatrix_[PlayerWeapon::Left_Shoulder] = skeleton->GetSkeletonSpaceMat("left_shoulder") * transform_->GetWorldMatrix();
	weaponBornMatrix_[PlayerWeapon::Right_Shoulder] = skeleton->GetSkeletonSpaceMat("right_shoulder") * transform_->GetWorldMatrix();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ ノックバック処理
///////////////////////////////////////////////////////////////////////////////////////////////

void Player::Knockback(const Math::Vector3& direction) {
	if (isKnockback_) { return; }
	Math::Vector3 dire = direction;

	// playerが地面についている際はy軸方向にノックバックしない用に対策する
	if (isLanding_) {
		dire.y = 0.f;
		dire = dire.Normalize();
	}

	// ノックバックさせる
	isKnockback_ = true;
	stateMachine_->ChangeState<PlayerKnockbackState>();

	size_t hash = typeid(PlayerActionDamaged).hash_code();
	actionManager_->ChangeAction(hash);

	knockBackDire_ = dire;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ エネルギーを回復する
///////////////////////////////////////////////////////////////////////////////////////////////

void Player::RecoveryEN(float timer) {
	if (isLanding_) {
		if (timer > param_.energyRecoveryCoolTime) {
			param_.energy += param_.energyRecoveryAmount * AOENGINE::GameTimer::DeltaTime();
			param_.energy = std::clamp(param_.energy, 0.0f, initParam_.energy);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ エネルギーを消費する
///////////////////////////////////////////////////////////////////////////////////////////////

void Player::ConsumeEN(float cousumeAmount) {
	param_.energy -= cousumeAmount;
	param_.energy = std::clamp(param_.energy, 0.0f, initParam_.energy);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ エネルギーを消費する
///////////////////////////////////////////////////////////////////////////////////////////////

void Player::Damage(float _damage) {
	if (isInvincible_) {
		return;
	} else {
		invincibleTimer_.timer_ = 0;
		isInvincible_ = true;
	}

	// hpを減らす
	psRecoveryTimer_.timer_ = 0.0f;
	param_.health -= _damage;
	if (param_.health <= 0.f) {
		isDead_ = true;
		stateMachine_->ChangeState<PlayerDeadState>();
	}
	// 姿勢安定性を減らす
	param_.postureStability += _damage * kPostureDamageRate;

	if (param_.postureStability >= initParam_.postureStability) {
		if (deployArmor_) {
			deployArmor_ = false;
			param_.postureStability = 0;
		} else {
			Knockback(transform_->GetRotate().MakeForward() * -1.0f);
		}
	}

	// カメラを揺らす
	pFollowCamera_->SetShake(kDamageShakeTime, kDamageShakeStrength);

	// ビネットを出す
	if (param_.health <= initParam_.health * param_.pinchOfPercentage) {
		vignette_->SetIsEnable(true);
		vignette_->SetPower(param_.pinchVignettePower);
		vignette_->SetColor(param_.pinchVignetteColor);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 着地時の処理
///////////////////////////////////////////////////////////////////////////////////////////////

void Player::Landing() {
	if (!isLanding_) {
		jet_->JetIsStop();
	}
	isLanding_ = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ BoostをOnにする
///////////////////////////////////////////////////////////////////////////////////////////////

void Player::IsBoostMode() {
	if (AOENGINE::Input::GetInstance()->IsTriggerButton(XInputButtons::ButtonB)) {
		jet_->SetIsBoostMode();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 足のCollider
///////////////////////////////////////////////////////////////////////////////////////////////

void Player::LegOnCollision([[maybe_unused]] AOENGINE::BaseCollider* other) {
	if (other->GetCategoryName() == "building" || other->GetCategoryName() == "ground") {
		Landing();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ アタック中なら
///////////////////////////////////////////////////////////////////////////////////////////////

bool Player::IsAttack() {
	if (isAttack_) {
		return true;
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ カメラを傾ける
///////////////////////////////////////////////////////////////////////////////////////////////

void Player::CameraIncline() {
	// スクリーン座標系でのX成分の差を求める
	Math::Vector3 screenPos = TransformCoord(CVector3::ZERO, transform_->GetWorldMatrix() * pFollowCamera_->GetVpvpMatrix());
	Math::Vector3 screenPosPrev = TransformCoord(CVector3::ZERO, transform_->GetWorldMatrixPrev() * pFollowCamera_->GetVpvpMatrix());
	screenPos_ = Math::Vector2(screenPos.x, screenPos.y);
	screenPosPrev_ = Math::Vector2(screenPosPrev.x, screenPosPrev.y);

	// ターゲット角度の生計算
	float diffX = std::abs(screenPos_.x) - std::abs(screenPosPrev_.x);
	if (std::abs(diffX) < param_.inclineThreshold) {
		diffX = 0;
	}
	diffX *= -1.0f; // 向きを反転

	// Clamp
	diffX = std::clamp(diffX, -param_.inclineStrength, param_.inclineStrength);

	// kは今回どれだけターゲットに寄せるかの係数
	// smoothSpeed_が大きいほど反応が速い
	float k = 1.0f - std::exp(-param_.inclineReactionRate * AOENGINE::GameTimer::DeltaTime());
	k = std::clamp(k, 0.0f, 1.0f);

	smoothedDiffX_ += (diffX - smoothedDiffX_) * k;

	// もし安全のため最終値も範囲内に留めたいなら
	smoothedDiffX_ = std::clamp(smoothedDiffX_, -param_.inclineStrength, param_.inclineStrength);

	pFollowCamera_->SetAngleZ(smoothedDiffX_);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 姿勢安定回復
///////////////////////////////////////////////////////////////////////////////////////////////

void Player::PostureStabilityRecovery() {
	if (param_.postureStability >= 0.0f) {
		if (!psRecoveryTimer_.Run(AOENGINE::GameTimer::DeltaTime())) {
			param_.postureStability -= param_.psRecoveryValue * AOENGINE::GameTimer::DeltaTime();
			param_.postureStability = std::clamp(param_.postureStability, 0.0f, initParam_.postureStability);
		} 
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ Objectの初期化
///////////////////////////////////////////////////////////////////////////////////////////////

void Player::InitObject() {
	object_ = AOENGINE::SceneRenderer::GetInstance()->GetGameObject<AOENGINE::BaseGameObject>("Player");
	AOENGINE::SceneRenderer::GetInstance()->ChangeRenderingType("Object_PBR.json", object_);
	transform_ = object_->GetTransform();
	object_->SetMaterial(MaterialType::PBR);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ アクション関連の初期化
///////////////////////////////////////////////////////////////////////////////////////////////

void Player::InitAction() {
	actionManager_ = std::make_unique<ActionManager<Player>>();
	actionManager_->Init(this, "PlayerAction");
	actionManager_->BuildAction<PlayerActionIdle>();
	actionManager_->BuildAction<PlayerActionMove>();
	actionManager_->BuildAction<PlayerActionJump>();
	actionManager_->BuildAction<PlayerActionQuickBoost>();
	actionManager_->BuildAction<PlayerActionBoost>();
	actionManager_->BuildAction<PlayerActionShotRight>();
	actionManager_->BuildAction<PlayerActionShotLeft>();
	actionManager_->BuildAction<PlayerActionRightShoulder>();
	actionManager_->BuildAction<PlayerActionLeftShoulder>();
	actionManager_->BuildAction<PlayerActionDamaged>();
	actionManager_->BuildAction<PlayerActionTurnAround>();
	actionManager_->BuildAction<PlayerActionDeployArmor>();

	size_t hash = typeid(PlayerActionIdle).hash_code();
	actionManager_->AddRunAction(hash);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ Colliderの初期化
///////////////////////////////////////////////////////////////////////////////////////////////

void Player::InitCollider() {
	AOENGINE::BaseCollider* collider = object_->GetCollider("player");
	collider->SetIsStatic(false);

	AOENGINE::BaseCollider* colliderLeftLeg = object_->GetCollider("playerLeftLeg");
	colliderLeftLeg->SetOnCollision([this](AOENGINE::BaseCollider* other) { LegOnCollision(other); });
	colliderLeftLeg->SetIsStatic(false);

	AOENGINE::BaseCollider* colliderRightLeg = object_->GetCollider("playerRightLeg");
	colliderRightLeg->SetOnCollision([this](AOENGINE::BaseCollider* other) { LegOnCollision(other); });
	colliderRightLeg->SetIsStatic(false);

	object_->SetPhysics();
	object_->GetRigidbody()->SetDrag(param_.windDrag);
}
