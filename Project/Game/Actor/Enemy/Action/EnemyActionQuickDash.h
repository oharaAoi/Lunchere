#pragma once
#include "Engine/System/AI/Node/BaseTaskNode.h"
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Engine/Lib/Math/Curve.h"

class BaseEnemy;

/// <summary>
/// Enemyのクイックダッシュ
/// </summary>
class EnemyActionQuickDash :
	public AI::BaseTaskNode<BaseEnemy> {
public:

	/// <summary>
	/// クイックダッシュ行動の調整パラメータ
	/// </summary>
	struct Parameter : public AOENGINE::IJsonConverter {
		float speed;
		float moveTime;
		Math::Curve curve;

		Parameter() {
			SetGroupName("EnemyAction");
			SetName("quickDash");
		}

		json ToJson(const std::string& id) const override {
			json curveData = curve.ToJson();
			return AOENGINE::JsonBuilder(id)
				.Add("speed", speed)
				.Add("moveTime", moveTime)
				.Add("curveData", curveData)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			Convert::fromJson(jsonData, "speed", speed);
			Convert::fromJson(jsonData, "moveTime", moveTime);
			curve.FromJson(jsonData, "curveData");
		}

		void Debug_Gui() override;
	};

public:

	EnemyActionQuickDash();
	~EnemyActionQuickDash() override = default;

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

	void Move();

private:

	Math::Vector3 moveDirection_;

	Parameter param_;

};

