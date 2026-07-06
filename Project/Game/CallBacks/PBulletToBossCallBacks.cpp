#include "PBulletToBossCallBacks.h"
#include "Engine/Render/SceneRenderer.h"
#include "Engine/System/Audio/AudioPlayer.h"
#include "Engine/System/Manager/ParticleManager.h"
#include "Game/Information/ColliderCategory.h"

namespace {
constexpr float kBulletHitVolume = 0.04f;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void PBulletToBossCallBacks::Init() {
	SetCallBacks();
	SetPair(pCollisionManager_, ColliderTags::Bullet::machinegun, ColliderTags::Boss::own);

	AOENGINE::ParticleManager* manager = AOENGINE::ParticleManager::GetInstance();
	hitBossSmoke_ = manager->CreateParticle("MachineGunHit");
	hitBossSmokeBorn_ = manager->CreateParticle("MachineGunHit2");
	hitSmoke_ = manager->CreateParticle("Expload");
	hitBossSmoke_->SetIsStop(true);
	hitBossSmokeBorn_->SetIsStop(true);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void PBulletToBossCallBacks::Update() {
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ コリジョン処理
///////////////////////////////////////////////////////////////////////////////////////////////

void PBulletToBossCallBacks::CollisionEnter([[maybe_unused]] AOENGINE::BaseCollider* const bullet, [[maybe_unused]] AOENGINE::BaseCollider* const boss) {
	BaseBullet* playerBullet = pBulletManager_->SearchCollider(bullet, ColliderTags::Bullet::machinegun);
	Math::Vector3 pos = playerBullet->GetTransform()->GetPreTranslate();
	// bulletの処理
	if (playerBullet != nullptr) {
		if (playerBullet->GetBulletType() == BulletType::Launcher) {
			hitSmoke_->SetPos(pos);
			hitSmoke_->Reset();
		}

		if (playerBullet->GetBulletType() != BulletType::Laser) {
			playerBullet->SetIsAlive(false);
		}

		// bossへの処理
		pBoss_->Damage(playerBullet->GetTakeDamage());
	}

	hitBossSmoke_->SetPos(pos);
	hitBossSmokeBorn_->SetPos(pos);

	hitBossSmoke_->SetIsStop(false);
	hitBossSmokeBorn_->SetIsStop(false);

	AOENGINE::AudioPlayer::SingleShotPlay("bulletHit.mp3", kBulletHitVolume);
}

void PBulletToBossCallBacks::CollisionStay([[maybe_unused]] AOENGINE::BaseCollider* const bullet, [[maybe_unused]] AOENGINE::BaseCollider* const boss) {
}

void PBulletToBossCallBacks::CollisionExit([[maybe_unused]] AOENGINE::BaseCollider* const bullet, [[maybe_unused]] AOENGINE::BaseCollider* const boss) {
}
