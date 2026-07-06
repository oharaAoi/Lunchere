#include "MachineGun.h"
#include "Engine/System/Manager/ParticleManager.h"
#include "Engine/System/Audio/AudioPlayer.h"
#include "Engine/System/Editor/Window/EditorWindows.h"
#include "Game/Actor/Player/Bullet/PlayerBullet.h"

namespace {
constexpr float kShotSeVolume = 0.06f;
}

void MachineGun::Finalize() {
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集
///////////////////////////////////////////////////////////////////////////////////////////////

void MachineGun::Debug_Gui() {
	BaseWeapon::Debug_Gui();
	ImGui::DragFloat3("offset", &offset_.x, .1f);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化
///////////////////////////////////////////////////////////////////////////////////////////////

void MachineGun::Init() {
	SetName("MachineGun");
	BaseWeapon::Init();
	attackParam_.SetName("MachineGunAttackParam");
	attackParam_.Load();
	
	object_->SetObject("gun.obj", MaterialType::PBR);
	transform_->SetRotate(Math::Quaternion::AngleAxis(kPI, CVector3::FORWARD) * Math::Quaternion::AngleAxis(kHPI, CVector3::RIGHT));

	// -------------------------------------------------
	// ↓ Effect関連
	// -------------------------------------------------
	gunFireParticles_ = AOENGINE::ParticleManager::GetInstance()->CreateParticle("gunFireParticles");
	gunFireParticles_->SetParent(transform_);

}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 弾を撃つ
///////////////////////////////////////////////////////////////////////////////////////////////

bool MachineGun::Attack(const AttackContext& cxt) {
	if (!isCanAttack_) { return false; }

	Math::Vector3 worldPos = object_->GetPosition();
	PlayerBullet* bullet = pBulletManager_->AddBullet<PlayerBullet>(worldPos, cxt.direction * attackParam_.bulletSpeed);
	bullet->SetTakeDamage(attackParam_.takeDamage);

	// effectを出す
	Math::Vector3 pos = offset_;
	pos = (cxt.direction * offset_.z);
	gunFireParticles_->SetPos(pos);
	gunFireParticles_->SetIsStop(false);

	AOENGINE::AudioPlayer::SingleShotPlay("shotBullet.mp3", kShotSeVolume);

	AttackAfter();
	return true;
}
