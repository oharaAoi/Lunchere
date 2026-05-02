#include "FlamethrowerToPlayerCallBacks.h"
#include "Game/Information/ColliderCategory.h"

void FlamethrowerToPlayerCallBacks::Init() {
	SetCallBacks();
	SetPair(pCollisionManager_, ColliderTags::Bullet::flamethrower, ColliderTags::Player::own);
}

void FlamethrowerToPlayerCallBacks::Update() {
}

void FlamethrowerToPlayerCallBacks::CollisionEnter([[maybe_unused]] AOENGINE::BaseCollider* const bullet, [[maybe_unused]] AOENGINE::BaseCollider* const player) {
	BaseBullet* pBullet = pBossBulletManager_->SearchCollider(bullet, ColliderTags::Bullet::flamethrower);
	// bulletの処理
	if (pBullet != nullptr) {
		pBullet->SetIsAlive(false);
		// bossへの処理
		pPlayer_->Damage(pBullet->GetTakeDamage());
	}
}

void FlamethrowerToPlayerCallBacks::CollisionStay([[maybe_unused]] AOENGINE::BaseCollider* const bullet, [[maybe_unused]] AOENGINE::BaseCollider* const player) {
}

void FlamethrowerToPlayerCallBacks::CollisionExit([[maybe_unused]] AOENGINE::BaseCollider* const bullet, [[maybe_unused]] AOENGINE::BaseCollider* const player) {
}
