#pragma once
#include "Engine/System/AI/GoalOriented/IOrientedGoal.h"
#include "Engine/Lib/Json/IJsonConverter.h"

/// <summary>
/// 敵を倒す目標
/// </summary>
class TargetDeadOriented :
	public AI::IOrientedGoal {
public: // データ構造体

	/// <summary>
	/// ターゲット死亡時の指向性評価パラメータ
	/// </summary>
	struct Consideration : public AOENGINE::IJsonConverter {
		float priority = 1.0f;		// 優先度
		float optimal = 80.0f;		// 適正距離
		float optimalRange = 20.0f;	// 適正距離許容範囲

		Consideration() {
			SetGroupName("AIConsideration");
			SetName("TargetDead");
		}

		json ToJson(const std::string& id) const override {
			return AOENGINE::JsonBuilder(id)
				.Add("priority", priority)
				.Add("optimal", optimal)
				.Add("optimalRange", optimalRange)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			Convert::fromJson(jsonData, "priority", priority);
			Convert::fromJson(jsonData, "optimal", optimal);
			Convert::fromJson(jsonData, "optimalRange", optimalRange);
		}

		void Debug_Gui() override;
	};

public: // コンストラクタ

	TargetDeadOriented();
	~TargetDeadOriented() override = default;

public:

	// 更新処理
	void Update() override {};

	// スコアの計算
	float CalculationScore() override;

	// 編集
	void Debug_Gui() override;

private:

	Consideration consideration_;

};

