#pragma once
#include "Engine/Lib/Math/MyMath.h"
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Engine/System/AI/Node/BaseTaskNode.h"
// game
#include "Game/Actor/Boss/BossFlamethrowers.h"

class Boss;

/// <summary>
/// Playerが背面にいる時の攻撃
/// </summary>
class BossActionBackSweep :
	public AI::BaseTaskNode<Boss> {
public:

	/// <summary>
	/// 背面薙ぎ払い攻撃の調整パラメータ
	/// </summary>
	struct Parameter : public AOENGINE::IJsonConverter {
		float threshold = 0.5f;		// 背面にいるかの閾値
		float rotateTime = 1.0f;	// 回転する時間
		int easeType;				// イージングの種類

		Parameter() {
			SetGroupName("BossAction");
			SetName("BossActionBackSweep");
		}

		json ToJson(const std::string& id) const override {
			return AOENGINE::JsonBuilder(id)
				.Add("threshold", threshold)
				.Add("rotateTime", rotateTime)
				.Add("easeType", easeType)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			Convert::fromJson(jsonData, "threshold", threshold);
			Convert::fromJson(jsonData, "rotateTime", rotateTime);
			Convert::fromJson(jsonData, "easeType", easeType);
		}

		void Debug_Gui() override;
	};

public:

	BossActionBackSweep();
	~BossActionBackSweep() override = default;

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

	void Sweep();

	bool CheckBehind();

	float CalcDot();

private:

	Parameter param_;

	Math::Quaternion startRotate_;
	Math::Quaternion endRotate_;
};

