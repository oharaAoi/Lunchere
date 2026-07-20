#pragma once
#include "Engine/Lib/Color.h"
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Game/State/ICharacterState.h"
#include "Game/Actor/Weapon/Armors.h"

class Boss;

/// <summary>
/// Bossがバリアを展開している状態
/// </summary>
class BossStateDeployArmor :
	public ICharacterState<Boss> {
public:

	/// <summary>
	/// ボスのアーマー展開状態パラメータ
	/// </summary>
	struct ArmorParameter : public AOENGINE::IJsonConverter {
		float durability = 100.0f;
		Math::Vector3 scale = CVector3::UNIT;
		AOENGINE::Color color = Colors::Linear::white;
		AOENGINE::Color edgeColor = Colors::Linear::white;

		Math::SRT uvTransform;

		ArmorParameter() { 
			SetGroupName("BossState");
			SetName("BossStateDeployArmor");
		}

		json ToJson(const std::string& id) const override {
			return AOENGINE::JsonBuilder(id)
				.Add("durability", durability)
				.Add("scale", scale)
				.Add("color", color)
				.Add("edgeColor", edgeColor)
				.Add("uvScale", uvTransform.scale)
				.Add("uvRotate", uvTransform.rotate)
				.Add("uvTranslate", uvTransform.translate)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			Convert::fromJson(jsonData, "durability", durability);
			Convert::fromJson(jsonData, "scale", scale);
			Convert::fromJson(jsonData, "color", color);			Convert::fromJson(jsonData, "edgeColor", edgeColor);
			Convert::fromJson(jsonData, "uvScale", uvTransform.scale);
			Convert::fromJson(jsonData, "uvRotate", uvTransform.rotate);
			Convert::fromJson(jsonData, "uvTranslate", uvTransform.translate);
		}

		void Debug_Gui() override;
	};

public:

	BossStateDeployArmor() = default;
	~BossStateDeployArmor() = default;

public:

	// 初期化
	void OnStart() override;
	// 更新
	void OnUpdate() override;
	// 終了時
	void OnExit() override;
	// 編集処理
	void Debug_Gui() override;

private:

	ArmorParameter armorParam_;

	Armors* armor_;

};

