#pragma once
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Engine/System/AI/Node/BaseTaskNode.h"

class Boss;

/// <summary>
/// Bossの距離を取る行動
/// </summary>
class BossActionKeepDistance :
	public AI::BaseTaskNode<Boss> {
public:

	/// <summary>
	/// ボスの距離維持行動パラメータ
	/// </summary>
	struct Parameter : public AOENGINE::IJsonConverter {
		float moveSpeed = 60.f;
		float moveTime = 2.0f;
		float getDistance = 10.0f;
		float decayRate = 4.0f;
		float rotateT = 0.1f;

		Parameter() { 
			SetGroupName("BossAction");
			SetName("bossKeepDistance");
		}

		json ToJson(const std::string& id) const override {
			return AOENGINE::JsonBuilder(id)
				.Add("moveSpeed", moveSpeed)
				.Add("moveTime", moveTime)
				.Add("getDistance", getDistance)
				.Add("decayRate", decayRate)
				.Add("rotateT", rotateT)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			Convert::fromJson(jsonData, "moveSpeed", moveSpeed);
			Convert::fromJson(jsonData, "moveTime", moveTime);
			Convert::fromJson(jsonData, "getDistance", getDistance);
			Convert::fromJson(jsonData, "decayRate", decayRate);
			Convert::fromJson(jsonData, "rotateT", rotateT);
		}

		void Debug_Gui() override;
	};

public:

	BossActionKeepDistance() = default;
	~BossActionKeepDistance() = default;

public:

	// 実行処理
	BehaviorStatus Execute() override;
	// weight値の計算
	float EvaluateWeight() override;
	// 編集処理
	void Debug_Gui() override;
	// 終了判定
	bool IsFinish() override;
	// Actionを実行できるかの確認
	bool CanExecute() override;
	// 初期化
	void Init() override;
	// 更新
	void Update() override;
	// 終了処理
	void End() override;

public:

	// 直線で離れる
	void Direct();

	// 旋回しながら離れる
	void Spin();

	// 止める
	void Stop();

private:

	Parameter param_;

	Math::Vector3 velocity_;
	Math::Vector3 accel_;

	int moveType_;

	// 旋回に必要な座標
	Math::Vector3 centerPos_;

	bool stopping_;

};

