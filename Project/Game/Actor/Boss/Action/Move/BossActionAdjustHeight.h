#pragma once
#include "Engine/System/AI/Node/BaseTaskNode.h"
#include "Engine/Lib/Math/Curve.h"
#include "Engine/Module/Components/Physics/Rigidbody.h"

class Boss;

/// <summary>
/// プレイヤーと高さをあわせる
/// </summary>
class BossActionAdjustHeight :
	public AI::BaseTaskNode<Boss> {
public:

	/// <summary>
	/// ボスの高度調整行動パラメータ
	/// </summary>
	struct Parameter : public AOENGINE::IJsonConverter {
		float speed;				// 速度
		float appropriateDistance;	// 適正距離
		float maxDistance;			// 最大距離
		float moveTime;				// 移動時間
		Math::Curve moveCurve;		// 移動のカーブ

		Parameter() { 
			SetGroupName("BossAction");
			SetName("BossActionAdjustHeight");
		}

		json ToJson(const std::string& id) const override {
			json curveData = moveCurve.ToJson();
			return AOENGINE::JsonBuilder(id)
				.Add("speed", speed)
				.Add("appropriateDistance", appropriateDistance)
				.Add("maxDistance", maxDistance)
				.Add("moveTime", moveTime)
				.Add("moveCurve", curveData)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			Convert::fromJson(jsonData, "speed", speed);
			Convert::fromJson(jsonData, "appropriateDistance", appropriateDistance);
			Convert::fromJson(jsonData, "maxDistance", maxDistance);
			Convert::fromJson(jsonData, "moveTime", moveTime);
			moveCurve.FromJson(jsonData, "moveCurve");
		}

		void Debug_Gui() override;
	};

public:

	BossActionAdjustHeight();
	~BossActionAdjustHeight() override = default;

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
	float speedY_;

	AOENGINE::Rigidbody* pRigidbody_ = nullptr;
};

