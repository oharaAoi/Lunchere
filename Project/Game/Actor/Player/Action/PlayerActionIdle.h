#pragma once
#include <memory>
#include "Game/Actor/Base/BaseAction.h"
#include "Engine/Lib/Json/IJsonConverter.h"

class Player;

/// <summary>
/// 待機アクション
/// </summary>
class PlayerActionIdle :
	public BaseAction<Player>{
public:

	/// <summary>
	/// 待機行動の調整パラメータ
	/// </summary>
	struct Parameter : public AOENGINE::IJsonConverter {
		float animationBlendTime = 0.2f;
		
		Parameter() { 
			SetGroupName("PlayerAction");
			SetName("actionIdle");
		}

		json ToJson(const std::string& id) const override {
			return AOENGINE::JsonBuilder(id)
				.Add("animationBlendTime", animationBlendTime)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			Convert::fromJson(jsonData, "animationBlendTime", animationBlendTime);
		}

		void Debug_Gui() override {};
	};

public:

	PlayerActionIdle() = default;
	~PlayerActionIdle() override = default;

public:

	// ビルド処理
	void Build() override;
	// 初期化
	void OnStart() override;
	// 更新
	void OnUpdate() override;
	// 終了処理
	void OnEnd() override;
	// 次のアクションの判定
	void CheckNextAction() override;
	// actionの入力判定
	bool IsInput() override;
	// 編集処理
	void Debug_Gui() override {};

private:

	Parameter param_;

};

