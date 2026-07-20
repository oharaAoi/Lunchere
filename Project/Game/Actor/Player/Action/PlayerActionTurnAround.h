#pragma once
#include "Engine/Lib/Math/Quaternion.h"
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Game/Actor/Base/BaseAction.h"

// 前方宣言
class Player;

/// <summary>
/// プレイヤーの振り向きアクション
/// </summary>
class PlayerActionTurnAround :
	public BaseAction<Player> {
public:

	/// <summary>
	/// 振り向き行動の調整パラメータ
	/// </summary>
	struct Parameter : public AOENGINE::IJsonConverter {
		float speed;
		float rotateTime;

		Parameter() { SetName("actionTurnAround"); }
		
		json ToJson(const std::string& id) const override {
			return AOENGINE::JsonBuilder(id)
				.Add("speed", speed)
				.Add("rotateTime", rotateTime)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			Convert::fromJson(jsonData, "speed", speed);
			Convert::fromJson(jsonData, "rotateTime", rotateTime);
		}

		void Debug_Gui() override;
	};
public:

	PlayerActionTurnAround() = default;
	~PlayerActionTurnAround() override {};

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
	void Debug_Gui() override;

private:	// action


	
private:

	// Parameter -------------------------------
	Parameter param_;
	
	const float kDeadZone_ = 0.1f;
	float speed_;
	Math::Vector3 direction_;

	Math::Quaternion targetRotate_;
	Math::Quaternion prevRotate_;

};

