#include "BBulletToGroundCallBacks.h"
#include "Engine/System/Manager/ParticleManager.h"
#include "Game/Information/ColliderCategory.h"

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BBulletToGroundCallBacks::Init() {
	SetCallBacks();
	SetPair(pCollisionManager_, ColliderTags::Boss::missile, ColliderTags::Field::ground);

	AOENGINE::ParticleManager* manager = AOENGINE::ParticleManager::GetInstance();
	hitEffect_ = manager->CreateParticle("MissileHit");
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BBulletToGroundCallBacks::Update() {
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ コリジョン処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BBulletToGroundCallBacks::CollisionEnter([[maybe_unused]] AOENGINE::BaseCollider* const bullet, [[maybe_unused]] AOENGINE::BaseCollider* const ground) {
	BaseBullet* hitBullet = pBossBulletManager_->SearchCollider(bullet, ColliderTags::Boss::missile);
	if (hitBullet != nullptr) {
		hitBullet->SetIsAlive(false);
		hitEffect_->SetPos(hitBullet->GetPosition());
		hitEffect_->Reset();
	}
}

void BBulletToGroundCallBacks::CollisionStay([[maybe_unused]] AOENGINE::BaseCollider* const bullet, [[maybe_unused]] AOENGINE::BaseCollider* const ground) {
}

void BBulletToGroundCallBacks::CollisionExit([[maybe_unused]] AOENGINE::BaseCollider* const bullet, [[maybe_unused]] AOENGINE::BaseCollider* const ground) {
}
