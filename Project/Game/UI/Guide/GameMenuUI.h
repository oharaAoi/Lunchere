#pragma once
#include "Engine/Module/Components/2d/Sprite.h"
#include "Engine/Module/Components/Attribute/AttributeGui.h"
#include "Engine/Utilities/Timer.h"
#include "Engine/Lib/Json/IJsonConverter.h"

enum class GameMenuSelectItem {
	Go_Title,
	Go_Game,
	kMax
};

/// <summary>
/// ゲーム中メニューUI
/// </summary>
class GameMenuUI :
	public AOENGINE::AttributeGui {
public:

	/// <summary>
	/// ゲーム中メニューUIの設定パラメータ
	/// </summary>
	struct Parameter : public AOENGINE::IJsonConverter {
		float inputCoolTime = 0.2f;
		AOENGINE::Color noSelectColor;

		Parameter() {
			SetGroupName("GameMenuUI");
			SetName("GameMenuUIParameter");
		}

		json ToJson(const std::string& id) const override {
			return AOENGINE::JsonBuilder(id)
				.Add("inputCoolTime", inputCoolTime)
				.Add("noSelectColor", noSelectColor)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			Convert::fromJson(jsonData, "inputCoolTime", inputCoolTime);
			Convert::fromJson(jsonData, "noSelectColor", noSelectColor);
		}

		void Debug_Gui() override;
	};

public:

	GameMenuUI() = default;
	~GameMenuUI();

public:

	void Init();

	void Update();

	void Debug_Gui() override;

	GameMenuSelectItem Decide();

private:

	void Select();


	void ChangeItem();

private:

	AOENGINE::Sprite* bg_;
	AOENGINE::Sprite* goTitle_;
	AOENGINE::Sprite* goGame_;

	GameMenuSelectItem item_;
	AOENGINE::Timer timer_;

	Parameter param_;
};

