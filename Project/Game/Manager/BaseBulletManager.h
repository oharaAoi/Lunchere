#pragma once
// c++
#include <list>
#include <memory>
// engine
#include <Module/Components/Collider/BaseCollider.h>
// game
#include "Game/Actor/Base/BaseBullet.h"

/// <summary>
/// bulletを管理するクラスの基底クラス
/// </summary>
class BaseBulletManager {
public: // コンストラクタ

	BaseBulletManager() = default;
	virtual ~BaseBulletManager() = default;

public:

	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Init() = 0;

	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update() = 0;

	/// <summary>
	/// Colliderのポインタからbulletのポインタを探索する
	/// </summary>
	/// <param name="collider">; コライダーのポインタ</param>
	/// <returns></returns>
	BaseBullet* SearchCollider(AOENGINE::BaseCollider* collider, const std::string& tag);

public: // member method

	/// <summary>
	/// Bossが弾を撃った後にリストに格納する
	/// </summary>
	/// <typeparam name="BulletType">: 弾種類</typeparam>
	/// <typeparam name="...Args">: 可変長引数</typeparam>
	/// <param name="...args">: 各バレットの構造体</param>
	template<typename BulletType, typename... Args>
	BulletType* AddBullet(Args&&... args) {
		auto& bullet = bulletList_.emplace_back(std::make_unique<BulletType>());
		auto type = static_cast<BulletType*>(bullet.get());
		type->Init();
		// BulletTypeに応じたReset呼び出し
		type->Reset(std::forward<Args>(args)...);

		return type;
	}

	/// <summary>
	/// Bulletの生存確認
	/// </summary>
	void CheckBulletLife();

protected:

	std::list<std::unique_ptr<BaseBullet>> bulletList_;


};

