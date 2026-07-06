#include "LockOnCommands.h"
#include "Engine/Render/Render.h"
#include "Engine/Lib/Math/Vector2.h"
#include "Engine/Lib/Math/Matrix4x4.h"
#include <limits>

LockOnCommand::LockOnCommand(EnemyManager* enemyManager, Reticle* reticle)
	: pEnemyManager_(enemyManager), pReticle_(reticle) {
}

void LockOnCommand::Execute() {
	// 最も距離が近い値を保持しておく
	float mostNearDistance = (std::numeric_limits<float>::max)();

	// reticleの座標
	Math::Vector2 reticlePos = pReticle_->GetPos();

	// 結果となる行列
	BaseEnemy* nearEnemy = nullptr;

	// 計算を行う
	for (const auto& enemy : pEnemyManager_->GetEnemyList()) {
		// 視界内に入っているなら
		if (enemy->GetIsWithinSight()) {
			// 敵のスクリーン座標
			Math::Vector2 enemyScreenPos = WorldToScreenCoordinate(
				enemy->GetTransform()->GetWorldMatrix(),
				AOENGINE::Render::GetVpvpMatrix()
			);

			float distance = (enemyScreenPos - reticlePos).Length();

			if (mostNearDistance > distance) {
				mostNearDistance = distance;
				nearEnemy = enemy.get();
			}
		}
	}

	if (nearEnemy) {
		pEnemyManager_->SetNearReticleEnemy(nearEnemy);
		pReticle_->LockOn();
	}
}
