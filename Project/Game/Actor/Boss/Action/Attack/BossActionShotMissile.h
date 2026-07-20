#pragma once
#include <memory>
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Engine/System/AI/Node/BaseTaskNode.h"

class Boss;

/// <summary>
/// ミサイルを放つ
/// </summary>
class BossActionShotMissile :
	public AI::BaseTaskNode<Boss> {
public:

	/// <summary>
	/// ミサイル発射攻撃の調整パラメータ
	/// </summary>
	struct Parameter : public AOENGINE::IJsonConverter {
		float shotInterval = 0.1f;
		float bulletSpeed = 100.0f;
		float takeDamage = 40.0f;
		float firstSpeedRaito = 0.2f;
		float trakingRaito = 0.5f;

		Parameter() {
			SetGroupName("BossAction");
			SetName("bossActionShotMissile");
		}

		json ToJson(const std::string& id) const override {
			return AOENGINE::JsonBuilder(id)
				.Add("shotInterval", shotInterval)
				.Add("bulletSpeed", bulletSpeed)
				.Add("takeDamage", takeDamage)
				.Add("firstSpeedRaito", firstSpeedRaito)
				.Add("trakingRaito", trakingRaito)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			Convert::fromJson(jsonData, "shotInterval", shotInterval);
			Convert::fromJson(jsonData, "bulletSpeed", bulletSpeed);
			Convert::fromJson(jsonData, "takeDamage", takeDamage);
			Convert::fromJson(jsonData, "firstSpeedRaito", firstSpeedRaito);
			Convert::fromJson(jsonData, "trakingRaito", trakingRaito);
		}

		void Debug_Gui() override;
	};

public:

	BossActionShotMissile() = default;
	~BossActionShotMissile() override = default;

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

private :
	Parameter param_;

	// 弾を打ち終わったかのフラグ
	bool isFinishShot_;

	bool attackStart_;

	const uint32_t kFireCount_ = 4;
	uint32_t fireCount_;

};