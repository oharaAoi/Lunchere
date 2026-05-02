#include "PBulletToEnemyCallBacks.h"
#include "Game/Information/ColliderCategory.h"
#include "Engine/System/Manager/ParticleManager.h"
#include "Engine/System/Audio/AudioPlayer.h"

void PBulletToEnemyCallBacks::Init() {
	SetCallBacks();
	SetPair(pCollisionManager_, ColliderTags::Bullet::machinegun, ColliderTags::Enemy::own);

	AOENGINE::ParticleManager* manager = AOENGINE::ParticleManager::GetInstance();
	hitBossSmoke_ = manager->CreateParticle("MachineGunHit");
	hitBossSmokeBorn_ = manager->CreateParticle("MachineGunHit2");
	hitSmoke_ = manager->CreateParticle("Expload");
	hitBossSmoke_->SetIsStop(true);
	hitBossSmokeBorn_->SetIsStop(true);
}

void PBulletToEnemyCallBacks::Update() {
}

void PBulletToEnemyCallBacks::CollisionEnter([[maybe_unused]] AOENGINE::BaseCollider* const bullet, [[maybe_unused]] AOENGINE::BaseCollider* const enemy) {
	BaseBullet* playerBullet = pBulletManager_->SearchCollider(bullet, ColliderTags::Bullet::machinegun);
	if (playerBullet != nullptr) {
		if (playerBullet->GetBulletType() == BulletType::Launcher) {
			hitSmoke_->SetPos(bullet->GetCenterPos());
			hitSmoke_->Reset();
		}

		if (playerBullet->GetBulletType() != BulletType::Laser) {
			playerBullet->SetIsAlive(false);
		}

		// bossへの処理
		BaseEnemy* pEnemy = pEnemyManager_->SearchCollider(enemy);
		if (pEnemy) {
			pEnemy->Damage(playerBullet->GetTakeDamage());
		}
	}

	hitBossSmoke_->SetPos(bullet->GetCenterPos());
	hitBossSmokeBorn_->SetPos(bullet->GetCenterPos());

	hitBossSmoke_->SetIsStop(false);
	hitBossSmokeBorn_->SetIsStop(false);

	AOENGINE::AudioPlayer::SingleShotPlay("bulletHit.mp3", 0.04f);
}

void PBulletToEnemyCallBacks::CollisionStay([[maybe_unused]] AOENGINE::BaseCollider* const bullet, [[maybe_unused]] AOENGINE::BaseCollider* const enemy) {
}

void PBulletToEnemyCallBacks::CollisionExit([[maybe_unused]] AOENGINE::BaseCollider* const bullet, [[maybe_unused]] AOENGINE::BaseCollider* const enemy) {
}
