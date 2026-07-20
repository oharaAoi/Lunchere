#pragma once
// engine
#include "Engine/Module/Components/Effect/BaseParticles.h"
#include "Engine/Module/Components/Collider/BaseCollider.h"
#include "Engine/Module/Components/Collider/BoxCollider.h"
// Game
#include "Game/Actor/Weapon/BaseWeapon.h"
#include "Game/Manager/BaseBulletManager.h"

enum class FlamethrowerAttackType {
	Bullet,
	Sword
};

/// <summary>
/// 火焔放射の武器
/// </summary>
class Flamethrower :
	public BaseWeapon {
public: // データ構造体

	/// <summary>
	/// 火炎放射器の調整パラメータ
	/// </summary>
	struct FlamethrowerParam : AOENGINE::IJsonConverter {
		Math::Vector3 colliderPos;
		Math::Vector3 colliderSize;
		Math::Vector3 pos;
		float bulletSpeed = 80.f;
		float bulletRadius = 4.f;
		float bulletDamage = 5.f;
		float shotInterval = 0.1f;

		FlamethrowerParam() {
			SetGroupName("Weapon");
			SetName("Flamethrower");
		}

		json ToJson(const std::string& id) const override {
			return AOENGINE::JsonBuilder(id)
				.Add("colliderPos", colliderPos)
				.Add("colliderSize", colliderSize)
				.Add("pos", pos)
				.Add("bulletSpeed", bulletSpeed)
				.Add("bulletRadius", bulletRadius)
				.Add("bulletDamage", bulletDamage)
				.Add("shotInterval", shotInterval)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			Convert::fromJson(jsonData, "colliderPos", colliderPos);
			Convert::fromJson(jsonData, "colliderSize", colliderSize);
			Convert::fromJson(jsonData, "pos", pos);
			Convert::fromJson(jsonData, "bulletSpeed", bulletSpeed);
			Convert::fromJson(jsonData, "bulletRadius", bulletRadius);
			Convert::fromJson(jsonData, "bulletDamage", bulletDamage);
			Convert::fromJson(jsonData, "shotInterval", shotInterval);
		}

		void Debug_Gui() override;
	};

public: // コンストラクタ

	Flamethrower() = default;
	~Flamethrower() override;

public:

	// 終了
	void Finalize() override;
	// 初期化
	void Init() override;
	// 編集
	void Debug_Gui() override;

	void ColliderLocalPosInverse();

public:	// member method

	// 攻撃
	bool Attack(const AttackContext& cxt) override;

public:

	void SetBulletManager(BaseBulletManager* manager) { pBulletManager_ = manager; }

	void SetIsAttack(bool isAttack);
	bool GetIsAttack() const;

	void SetAttackType(FlamethrowerAttackType type) { attackType_ = type; }

private:

	BaseBulletManager* pBulletManager_ = nullptr;

	FlamethrowerParam flamethrowerParam_;

	AOENGINE::BaseParticles* flameParticle_ = nullptr;

	AOENGINE::BoxCollider* collider_ = nullptr;

	float attackTimer_ = 0;

	FlamethrowerAttackType attackType_;

};

