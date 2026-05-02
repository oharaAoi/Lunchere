#include "RocketToBossCallBacks.h"
#include "Game/Information/ColliderCategory.h"

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void RocketToBossCallBacks::Init() {
	SetCallBacks();
	SetPair(pCollisionManager_, ColliderTags::Bullet::rocket, ColliderTags::Boss::own);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void RocketToBossCallBacks::Update() {
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ コリジョン処理
///////////////////////////////////////////////////////////////////////////////////////////////

void RocketToBossCallBacks::CollisionEnter([[maybe_unused]] AOENGINE::BaseCollider* const bullet, [[maybe_unused]] AOENGINE::BaseCollider* const boss) {
	BaseBullet* playerBullet = pBulletManager_->SearchCollider(bullet, ColliderTags::Bullet::rocket);
	// bulletの処理
	if (playerBullet != nullptr) {
		playerBullet->SetIsAlive(false);
		// bossへの処理
		pBoss_->Damage(playerBullet->GetTakeDamage());
	}
}

void RocketToBossCallBacks::CollisionStay([[maybe_unused]] AOENGINE::BaseCollider* const bullet, [[maybe_unused]] AOENGINE::BaseCollider* const boss) {
}

void RocketToBossCallBacks::CollisionExit([[maybe_unused]] AOENGINE::BaseCollider* const bullet, [[maybe_unused]] AOENGINE::BaseCollider* const boss) {
}
