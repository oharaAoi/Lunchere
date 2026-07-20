#pragma once
#include <memory>
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Engine/Module/Components/Attribute/AttributeGui.h"
#include "Engine/Module/Components/2d/Sprite.h"
#include "Engine/Module/Components/Animation/VectorTween.h"

/// <summary>
/// /Clear後の通知を行ってくれるUI
/// </summary>
class ClearNotificationUI :
	public AOENGINE::AttributeGui {
public:

	/// <summary>
	/// クリア通知UIの移動演出パラメータ
	/// </summary>
	struct Parameter : public AOENGINE::IJsonConverter {
		Math::Vector2 startPos;
		Math::Vector2 endPos;
		float duration;

		Parameter() { 
			SetGroupName("CanvasUI");
			SetName("clearNotificationUI");
		}

		json ToJson(const std::string& id) const override {
			return AOENGINE::JsonBuilder(id)
				.Add("startPos", startPos)
				.Add("endPos", endPos)
				.Add("duration", duration)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			Convert::fromJson(jsonData, "startPos", startPos);
			Convert::fromJson(jsonData, "endPos", endPos);
			Convert::fromJson(jsonData, "duration", duration);
		}

		void Debug_Gui() override;
	};

public:

	ClearNotificationUI() = default;
	~ClearNotificationUI() = default;

public:

	// 初期化
	void Init();
	
	// 更新
	void Update(bool isBossBreak);

	// 描画
	void Draw() const;

	// 編集
	void Debug_Gui() override;

public:

	void Reset();

	bool GetIsBreak() const { return isBreak_; }

	bool IsFinish() { return posTween_.GetIsFinish(); }

	AOENGINE::Sprite* GetSprite() const { return sprite_; }

private:

	bool isBreak_ = false;

	AOENGINE::Sprite* sprite_;
	Parameter param_;

	Math::Vector2 pos_;
	AOENGINE::VectorTween<Math::Vector2> posTween_;

	AOENGINE::VectorTween<float> alphaTween_;

};

