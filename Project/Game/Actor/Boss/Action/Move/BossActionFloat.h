#pragma once
#include "Engine/System/AI/Node/BaseTaskNode.h"
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Engine/Lib/Math/Curve.h"

class Boss;


/// <summary>
/// ボスの浮遊行動
/// </summary>
class BossActionFloat :
	public AI::BaseTaskNode<Boss>{

public:

	/// <summary>
	/// ボスの浮遊行動パラメータ
	/// </summary>
	struct Parameter : public AOENGINE::IJsonConverter {
		float moveTime = 0.5f;		// 移動時間
		float moveSpeed = 10.0f;	// 移動速度
		float appropriateDistance;	// 適正距離
		float maxDistance;			// 最大距離
		Math::Curve moveCurve;

		Parameter() { 
			SetGroupName("BossAction");
			SetName("BossActionFloat"); 
		}

		json ToJson(const std::string& id) const override {
			json curveJson = moveCurve.ToJson();
			return AOENGINE::JsonBuilder(id)
				.Add("moveTime", moveTime)
				.Add("moveSpeed", moveSpeed)
				.Add("appropriateDistance", appropriateDistance)
				.Add("maxDistance", maxDistance)
				.Add("moveCurve", curveJson)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			Convert::fromJson(jsonData, "moveTime", moveTime);
			Convert::fromJson(jsonData, "moveSpeed", moveSpeed);
			Convert::fromJson(jsonData, "appropriateDistance", appropriateDistance);
			Convert::fromJson(jsonData, "maxDistance", maxDistance);
			moveCurve.FromJson(jsonData, "moveCurve");
		}

		void Debug_Gui() override;
	};

public:

	BossActionFloat();
	~BossActionFloat() override = default;

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
	Parameter param_;
};

