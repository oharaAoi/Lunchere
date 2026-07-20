#pragma once
// Engine
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Engine/Module/Components/GameObject/BaseEntity.h"
#include "Game/Manager/BaseBulletManager.h"

/// <summary>
/// 攻撃時に渡す方向とターゲット情報
/// </summary>
struct AttackContext {
	Math::Vector3 direction;
	Math::Vector3 target;
};

/// <summary>
/// 武器のベースとなるクラス
/// </summary>
class BaseWeapon :
	public AOENGINE::BaseEntity {
public:

	/// <summary>
	/// 武器攻撃の共通パラメータ
	/// </summary>
	struct AttackParam : public AOENGINE::IJsonConverter {
		float bulletSpeed = 100;	// 弾の速さ
		int maxBulletsNum = 400;	// 最大弾数
		int fireBulletsNum = 50;	// マガジンの量
		float fireInterval = 0.3f;	// 発射間隔
		float reloadTime = 2.0f;	// リロード時間
		float takeDamage = 20;		// ダメージを与える量
		float startUpTime = 0.0f;	// 攻撃前の準備時間
		float recoilTime = 0.0f;	// 攻撃後の硬直時間

		AttackParam() {
			SetName("AttackParam");
			SetGroupName("Weapon");
		}

		json ToJson(const std::string& id) const override {
			return AOENGINE::JsonBuilder(id)
				.Add("bulletSpeed", bulletSpeed)
				.Add("maxBulletsNum", maxBulletsNum)
				.Add("fireBulletsNum", fireBulletsNum)
				.Add("fireInterval", fireInterval)
				.Add("reloadTime", reloadTime)
				.Add("takeDamage", takeDamage)
				.Add("startUpTime", startUpTime)
				.Add("recoilTime", recoilTime)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			Convert::fromJson(jsonData, "bulletSpeed", bulletSpeed);
			Convert::fromJson(jsonData, "maxBulletsNum", maxBulletsNum);
			Convert::fromJson(jsonData, "fireBulletsNum", fireBulletsNum);
			Convert::fromJson(jsonData, "fireInterval", fireInterval);
			Convert::fromJson(jsonData, "reloadTime", reloadTime);
			Convert::fromJson(jsonData, "takeDamage", takeDamage);
			Convert::fromJson(jsonData, "startUpTime", startUpTime);
			Convert::fromJson(jsonData, "recoilTime", recoilTime);
		}

		void Debug_Gui() override;
	};

public:

	BaseWeapon() = default;
	virtual ~BaseWeapon() = default;

public:

	// 終了
	virtual void Finalize();
	// 初期化
	virtual void Init();
	// 更新
	virtual void Update();
	virtual void PostUpdate();
	// 編集
	void Debug_Gui() override;

public:		// member method

	// 攻撃後時間
	void AttackAfter();

	// 攻撃関数
	virtual bool Attack(const AttackContext& cxt) = 0;

	// 残弾数の割合
	float BulletsFill();

	// リロード時間の表示
	float ReloadFill();

public:		// accessor method

	void SetBulletManager(BaseBulletManager* _bulletManager) { pBulletManager_ = _bulletManager; }

	bool GetIsReload() const { return isReload_; }

	float GetStartUpTime() const { return attackParam_.startUpTime; }
	float GetRecoilTime() const { return attackParam_.recoilTime; }

protected:	// 

	BaseBulletManager* pBulletManager_ = nullptr;

	AttackParam attackParam_;

	// 攻撃に関するパラメータ
	bool isCanAttack_ = true;
	bool isReload_ = false;
	float coolTime_ = 0.0f;
	int fireCount_ = 0;

};

