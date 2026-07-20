#pragma once
#include <string>
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Engine/Module/Components/Animation/VectorTween.h"
#include "Game/UI/BaseGaugeUI.h"

enum class GaugeType {
	Posturebility,
	Armor,
	Stun
};

/// <summary>
/// 姿勢安定のゲージ
/// </summary>
class PostureStability :
	public BaseGaugeUI {
public:

	/// <summary>
	/// 姿勢安定ゲージの表示色パラメータ
	/// </summary>
	struct Parameter : public AOENGINE::IJsonConverter {
		AOENGINE::Color normalColor;
		AOENGINE::Color pinchColor;
		AOENGINE::Color armorColor;
		AOENGINE::Color stunColor;

		Parameter() { SetName("postureStabilityColor"); }

		json ToJson(const std::string& id) const override {
			return AOENGINE::JsonBuilder(id)
				.Add("normalColor", normalColor)
				.Add("pinchColor", pinchColor)
				.Add("armorColor", armorColor)
				.Add("stunColor", stunColor)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			Convert::fromJson(jsonData, "normalColor", normalColor);
			Convert::fromJson(jsonData, "pinchColor", pinchColor);
			Convert::fromJson(jsonData, "armorColor", armorColor);
			Convert::fromJson(jsonData, "stunColor", stunColor);
		}

		void Debug_Gui() override {};
	};

public:

	PostureStability() = default;
	~PostureStability() override = default;

public:

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="_groupName"></param>
	/// <param name="_itemName"></param>
	void Init(const std::string& _groupName, const std::string& _itemName);
	
	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="_fillAmount"></param>
	void Update(float _fillAmount);

	/// <summary>
	/// 編集
	/// </summary>
	void Debug_Gui() override;

public:

	void SetGaugeType(GaugeType _type) { gaugeType_ = _type; }

private:

	Parameter param_;

	GaugeType gaugeType_;

	AOENGINE::VectorTween<AOENGINE::Color> stunAnimation_;
};

