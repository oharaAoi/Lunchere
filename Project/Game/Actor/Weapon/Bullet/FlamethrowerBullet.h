#pragma once
// engine
#include "Engine/Utilities/Timer.h"
// game
#include "Game/Actor/Base/BaseBullet.h"

/// <summary>
/// 火炎放射器から発射される弾
/// </summary>
class FlamethrowerBullet :
	public BaseBullet {
public:

	FlamethrowerBullet() = default;
	~FlamethrowerBullet() override;

public:

	/// <summary>
	/// 初期処理
	/// </summary>
	void Init();

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update() override;

	/// <summary>
	/// 
	/// </summary>
	/// <param name="pos"></param>
	/// <param name="direction"></param>
	/// <param name="speed"></param>
	/// <param name="radius"></param>
	void Reset(const Math::Vector3& pos, const Math::Vector3& direction,
			   float speed, float radius);

	/// <summary>
	/// 衝突時の処理
	/// </summary>
	/// <param name="other"></param>
	void OnCollision(AOENGINE::BaseCollider* other);

private:

	const float kLifeTime_ = 1.0f;
	AOENGINE::Timer timer_;

	Math::Vector3 direction_;
};

