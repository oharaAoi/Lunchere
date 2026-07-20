#pragma once
#include <memory>
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Engine/System/AI/Node/BaseTaskNode.h"

class Boss;

/// <summary>
/// ランチャーを撃つ
/// </summary>
class BossActionShotLauncher :
	public AI::BaseTaskNode<Boss> {
public:

	/// <summary>
	/// ランチャー発射攻撃の調整パラメータ
	/// </summary>
	struct Parameter : public AOENGINE::IJsonConverter {
		float bulletSpeed = 80.0f;
		float stiffenTime = 1.0f;
		float takeDamage = 40.0f;
		float firstSpeedRaito = 0.2f;
		float trakingRaito = 0.5f;

		Parameter() { 
			SetGroupName("BossAction"); 
			SetName("BossActionShotLuncher");
		}
		
		json ToJson(const std::string& id) const override {
			return AOENGINE::JsonBuilder(id)
				.Add("bulletSpeed", bulletSpeed)
				.Add("stiffenTime", stiffenTime)
				.Add("takeDamage", takeDamage)
				.Add("firstSpeedRaito", firstSpeedRaito)
				.Add("trakingRaito", trakingRaito)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			Convert::fromJson(jsonData, "bulletSpeed", bulletSpeed);
			Convert::fromJson(jsonData, "stiffenTime", stiffenTime);
			Convert::fromJson(jsonData, "takeDamage", takeDamage);
			Convert::fromJson(jsonData, "firstSpeedRaito", firstSpeedRaito);
			Convert::fromJson(jsonData, "trakingRaito", trakingRaito);
		}

		void Debug_Gui() override;
	};

public:

	BossActionShotLauncher() = default;
	~BossActionShotLauncher() override = default;

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

	// 攻撃
	void Shot();

private:

	Parameter param_;

	bool isFinish_;
	bool attackStart_;

};

