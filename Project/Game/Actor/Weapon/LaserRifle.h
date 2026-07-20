#pragma once
// Game
#include "Game/Actor/Weapon/BaseWeapon.h"

/// <summary>
/// レーザー系の武器
/// </summary>
class LaserRifle :
	public BaseWeapon {
public: // データ構造体

	/// <summary>
	/// レーザーライフルの調整パラメータ
	/// </summary>
	struct LaserRifleParam : AOENGINE::IJsonConverter {
		Math::Vector3 pos;

		LaserRifleParam() {
			SetGroupName("Weapon");
			SetName("LaserRifle");
		}

		json ToJson(const std::string& id) const override {
			return AOENGINE::JsonBuilder(id)
				.Add("pos", pos)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			Convert::fromJson(jsonData, "pos", pos);
		}

		void Debug_Gui() override;
	};

public: // コンストラクタ

	LaserRifle() = default;
	~LaserRifle() override = default;

public:

	// 終了
	void Finalize() override;
	// 初期化
	void Init() override;
	// 編集
	void Debug_Gui() override;

public:		// member method

	// 攻撃
	bool Attack(const AttackContext& cxt) override;

private:

	LaserRifleParam weaponParam_;

};

