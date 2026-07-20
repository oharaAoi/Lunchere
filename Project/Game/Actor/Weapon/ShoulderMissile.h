#pragma once
#include "Engine/Lib/Json/IJsonConverter.h"
// Game
#include "Game/Actor/Weapon/BaseWeapon.h"

/// <summary>
/// 肩の武器
/// </summary>
class ShoulderMissile :
	public BaseWeapon {
public:

	/// <summary>
	/// 肩ミサイルの調整パラメータ
	/// </summary>
	struct ShoulderMissileParam : AOENGINE::IJsonConverter {
		Math::Vector3 pos;
		float trackingLength;	// 追従する長さ
		float trackingTime;		// 追従する時間
		float trackingRaito;	// 追従の割合

		float shotSeVolum;		// 発射音の大きさ

		ShoulderMissileParam() {
			SetGroupName("Weapon");
			SetName("shoulderMissile");
		}

		json ToJson(const std::string& id) const override {
			return AOENGINE::JsonBuilder(id)
				.Add("pos", pos)
				.Add("trackingLength", trackingLength)
				.Add("trackingTime", trackingTime)
				.Add("trackingRaito", trackingRaito)
				.Add("shotSeVolum", shotSeVolum)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			Convert::fromJson(jsonData, "pos", pos);
			Convert::fromJson(jsonData, "trackingLength", trackingLength);
			Convert::fromJson(jsonData, "trackingTime", trackingTime);
			Convert::fromJson(jsonData, "trackingRaito", trackingRaito);
			Convert::fromJson(jsonData, "shotSeVolum", shotSeVolum);
		}

		void Debug_Gui() override;
	};

public:

	ShoulderMissile() = default;
	~ShoulderMissile() override = default;

public:

	// 終了
	void Finalize() override;
	// 初期化
	void Init() override;
	// 更新
	void Update() override;
	// 編集
	void Debug_Gui() override;

public:		// member method

	// 攻撃
	bool Attack(const AttackContext& cxt) override;

	// 弾を打つ処理
	void Shot();

private:


	ShoulderMissileParam weaponParam_;

	AttackContext attackCxt_;
	bool isFinish_;

	bool isLockOn_ = true;;
};

