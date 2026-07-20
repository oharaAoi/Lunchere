#pragma once
#include <memory>
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Engine/System/AI/Node/BaseTaskNode.h"

class Boss;

/// <summary>
/// 通常弾を撃つ
/// </summary>
class BossActionShotBullet :
	public AI::BaseTaskNode<Boss> {
public:

	/// <summary>
	/// 通常弾発射攻撃の調整パラメータ
	/// </summary>
	struct Parameter : public AOENGINE::IJsonConverter {
		float shotInterval = 0.1f;
		float bulletSpeed = 80.0f;
		int kFireCount = 20;
		float takeDamage = 10.0f;

		Parameter() { 
			SetGroupName("BossAction");
			SetName("bossActionShotBullet");
		}
		
		json ToJson(const std::string& id) const override {
			return AOENGINE::JsonBuilder(id)
				.Add("shotInterval", shotInterval)
				.Add("bulletSpeed", bulletSpeed)
				.Add("kFireCount", kFireCount)
				.Add("takeDamage", takeDamage)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			Convert::fromJson(jsonData, "shotInterval", shotInterval);
			Convert::fromJson(jsonData, "bulletSpeed", bulletSpeed);
			Convert::fromJson(jsonData, "kFireCount", kFireCount);
			Convert::fromJson(jsonData, "takeDamage", takeDamage);
		}

		void Debug_Gui() override;
	};
public:

	BossActionShotBullet() = default;
	~BossActionShotBullet() override = default;

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

	bool attackStart_;
	bool isFinishShot_;
	int fireCount_;

};

