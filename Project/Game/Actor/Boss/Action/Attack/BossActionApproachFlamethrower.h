#pragma once
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Engine/System/AI/Node/BaseTaskNode.h"
// game
#include "Game/Actor/Boss/BossFlamethrowers.h"

class Boss;

/// <summary>
/// ボスが近づきながら火炎放射で攻撃を行なってくる行動
/// </summary>
class BossActionApproachFlamethrower :
	public AI::BaseTaskNode<Boss> {
public:

	/// <summary>
	/// 接近火炎放射攻撃の移動パラメータ
	/// </summary>
	struct Parameter : public AOENGINE::IJsonConverter {
		float closeLength;		// 閉じ始める距離の長さ
		float stopLength;		// 減速を開始する距離
		float dampingValue;		// 減速量
		float speed;			// 速度

		Parameter() {
			SetGroupName("BossAction");
			SetName("bossActionApproachFlamethrower");
		}

		json ToJson(const std::string& id) const override {
			return AOENGINE::JsonBuilder(id)
				.Add("closeLength", closeLength)
				.Add("stopLength", stopLength)
				.Add("dampingValue", dampingValue)
				.Add("speed", speed)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			Convert::fromJson(jsonData, "closeLength", closeLength);
			Convert::fromJson(jsonData, "stopLength", stopLength);
			Convert::fromJson(jsonData, "dampingValue", dampingValue);
			Convert::fromJson(jsonData, "speed", speed);
		}

		void Debug_Gui() override;
	};

	/// <summary>
	/// 接近火炎放射攻撃の射撃パラメータ
	/// </summary>
	struct AttackFlamethrower : public AOENGINE::IJsonConverter {
		float closeTime = 1.0f;
		float startAngle = 90;
		float endAngle = 0;
		int easeType = 1;

		AttackFlamethrower() {
			SetGroupName("BossAction");
			SetName("approachAttackFlamethrower");
		}

		json ToJson(const std::string& id) const override {
			return AOENGINE::JsonBuilder(id)
				.Add("closeTime", closeTime)
				.Add("startAngle", startAngle)
				.Add("endAngle", endAngle)
				.Add("easeType", easeType)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			Convert::fromJson(jsonData, "closeTime", closeTime);
			Convert::fromJson(jsonData, "startAngle", startAngle);
			Convert::fromJson(jsonData, "endAngle", endAngle);
			Convert::fromJson(jsonData, "easeType", easeType);
		}

		void Debug_Gui() override;
	};

public:

	BossActionApproachFlamethrower();
	~BossActionApproachFlamethrower() override = default;

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

	// 敵に近づいて来る
	void Approach();

	/// <summary>
	/// 火炎放射を閉じる関数
	/// </summary>
	void CloseFlamethrowers();

private:

	Parameter param_;
	AttackFlamethrower flamethrowerParam_;

	Math::Vector3 direction_;
	Math::Vector3 velocity_;
	float speed_;
	bool isDamping_ = false;

	BossFlamethrowers* pFlamethrowers_;

	AOENGINE::Timer closeTimer_;
	bool isClose_ = false;
};

