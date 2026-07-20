#pragma once
#include "Engine/System/AI/Node/BaseTaskNode.h"
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Engine/Lib/Math/Vector3.h"

class BaseEnemy;

/// <summary>
/// 敵の歩く行動
/// </summary>
class EnemyActionWalk :
	public AI::BaseTaskNode<BaseEnemy> {
public: // データ

	/// <summary>
	/// 歩行行動の調整パラメータ
	/// </summary>
	struct Parameter : public AOENGINE::IJsonConverter {
		float speed;
		float changeDirectionTime;
		
		Parameter() {
			SetGroupName("EnemyAction");
			SetName("walk");
		}

		json ToJson(const std::string& id) const override {
			return AOENGINE::JsonBuilder(id)
				.Add("speed", speed)
				.Add("changeDirectionTime", changeDirectionTime)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			Convert::fromJson(jsonData, "speed", speed);
			Convert::fromJson(jsonData, "changeDirectionTime", changeDirectionTime);
		}

		void Debug_Gui() override;
	};

public: // コンストラクタ

	EnemyActionWalk() = default;
	~EnemyActionWalk() override = default;

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

	Math::Vector3 direction_;

};

