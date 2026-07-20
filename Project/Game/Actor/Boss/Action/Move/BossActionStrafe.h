#pragma once
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Engine/Lib/Math/Curve.h"
#include "Engine/System/AI/Node/BaseTaskNode.h"

class Boss;

/// <summary>
/// 小刻みに横に動く移動
/// </summary>
class BossActionStrafe :
	public AI::BaseTaskNode<Boss> {
public:

	/// <summary>
	/// ボスの横移動行動パラメータ
	/// </summary>
	struct Parameter : public AOENGINE::IJsonConverter {
		float moveSpeed = 60.f;
		float moveTime = 0.5f;
		float getDistance = 10.0f;
		float decayRate = 4.0f;
		float rotateT = 0.1f;
		float finishDistance = 1.0f;
		Math::Curve curve;

		Parameter() { 
			SetGroupName("BossAction");
			SetName("bossStrafe");
		}

		json ToJson(const std::string& id) const override {
			json curveData = curve.ToJson();
			return AOENGINE::JsonBuilder(id)
				.Add("moveSpeed", moveSpeed)
				.Add("moveTime", moveTime)
				.Add("getDistance", getDistance)
				.Add("decayRate", decayRate)
				.Add("rotateT", rotateT)
				.Add("finishDistance", finishDistance)
				.Add("curveData", curveData)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			Convert::fromJson(jsonData, "moveSpeed", moveSpeed);
			Convert::fromJson(jsonData, "moveTime", moveTime);
			Convert::fromJson(jsonData, "getDistance", getDistance);
			Convert::fromJson(jsonData, "decayRate", decayRate);
			Convert::fromJson(jsonData, "rotateT", rotateT);
			Convert::fromJson(jsonData, "finishDistance", finishDistance);
			curve.FromJson(jsonData, "curveData");
		}

		void Debug_Gui() override;
	};

public:

	BossActionStrafe() = default;
	~BossActionStrafe() = default;

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

private:

	// 旋回処理
	void Spin();

	// 止まる処理
	void Stop();

private:

	Parameter param_;

	Math::Vector3 velocity_;
	Math::Vector3 accel_;

	bool stopping_;
};

