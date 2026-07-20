#pragma once
#include "Engine/System/AI/Node/BaseTaskNode.h"
#include "Engine/Lib/Json/IJsonConverter.h"

class BaseEnemy;

/// <summary>
/// 敵が近づくアクション
/// </summary>
class EnemyActionApproach :
	public AI::BaseTaskNode<BaseEnemy> {
public:

	/// <summary>
	/// 接近行動の調整パラメータ
	/// </summary>
	struct Parameter : public AOENGINE::IJsonConverter {
		float speed;
		float limitDistance;

		Parameter() {
			SetGroupName("EnemyAction");
			SetName("approach");
		}

		json ToJson(const std::string& id) const override {
			return AOENGINE::JsonBuilder(id)
				.Add("speed", speed)
				.Add("limitDistance", limitDistance)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			Convert::fromJson(jsonData, "speed", speed);
			Convert::fromJson(jsonData, "limitDistance", limitDistance);
		}

		void Debug_Gui() override;
	};

public:

	EnemyActionApproach() = default;
	~EnemyActionApproach() override = default;

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

