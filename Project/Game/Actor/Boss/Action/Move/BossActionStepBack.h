#pragma once
#include "Engine/System/AI/Node/BaseTaskNode.h"
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Engine/Lib/Math/Curve.h"

class Boss;

/// <summary>
/// ボスの後退行動
/// </summary>
class BossActionStepBack :
	public AI::BaseTaskNode<Boss> {
public:

	/// <summary>
	/// ボスの後退行動パラメータ
	/// </summary>
	struct Parameter : public AOENGINE::IJsonConverter {
		float speed = 0;
		float moveTime = 0;
		float minDecel = 1.0f;
		float maxDecel = 12.0f;
		float stopThreshold = 12.0f;
		Math::Curve decelCurve;

		Parameter() {
			SetGroupName("BossAction");
			SetName("BossActionStepBack");
		}

		json ToJson(const std::string& id) const override {
			json curveJson = decelCurve.ToJson();
			return AOENGINE::JsonBuilder(id)
				.Add("speed", speed)
				.Add("moveTime", moveTime)
				.Add("minDecel", minDecel)
				.Add("maxDecel", maxDecel)
				.Add("stopThreshold", stopThreshold)
				.Add("decelCurve", curveJson)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			Convert::fromJson(jsonData, "speed", speed);
			Convert::fromJson(jsonData, "moveTime", moveTime);
			Convert::fromJson(jsonData, "minDecel", minDecel);
			Convert::fromJson(jsonData, "maxDecel", maxDecel);
			Convert::fromJson(jsonData, "stopThreshold", stopThreshold);
			decelCurve.FromJson(jsonData, "decelCurve");
		}

		void Debug_Gui() override;
	};

public:

	BossActionStepBack();
	~BossActionStepBack() override = default;

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

	Math::Vector3 direction_;
	Math::Vector3 velocity_;
	Parameter param_;

	
};

