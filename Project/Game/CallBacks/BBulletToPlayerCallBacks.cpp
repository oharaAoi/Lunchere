#include "BBulletToPlayerCallBacks.h"
#include "Game/Information/ColliderCategory.h"

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BBulletToPlayerCallBacks::Init() {
	SetCallBacks();
	SetPair(pCollisionManager_, ColliderTags::Boss::missile, ColliderTags::Player::own);

	AOENGINE::ParticleManager* manager = AOENGINE::ParticleManager::GetInstance();
	hitEffect_ = manager->CreateParticle("MissileHit");
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BBulletToPlayerCallBacks::Update() {
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ コリジョン処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BBulletToPlayerCallBacks::CollisionEnter([[maybe_unused]] AOENGINE::BaseCollider* const bullet, [[maybe_unused]] AOENGINE::BaseCollider* const player) {
	BaseBullet* hitBullet = pBossBulletManager_->SearchCollider(bullet, ColliderTags::Boss::missile);
	if (hitBullet != nullptr) {
		hitBullet->SetIsAlive(false);
		hitEffect_->SetPos(hitBullet->GetPosition());
		hitEffect_->Reset();
		pPlayer_->Damage(hitBullet->GetTakeDamage());
	}
}

void BBulletToPlayerCallBacks::CollisionStay([[maybe_unused]] AOENGINE::BaseCollider* const bullet, [[maybe_unused]] AOENGINE::BaseCollider* const player) {
}

void BBulletToPlayerCallBacks::CollisionExit([[maybe_unused]] AOENGINE::BaseCollider* const bullet, [[maybe_unused]] AOENGINE::BaseCollider* const player) {
}
