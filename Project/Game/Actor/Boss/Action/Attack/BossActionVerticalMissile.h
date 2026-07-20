#pragma once
#include "Engine/System/AI/Node/BaseTaskNode.h"
#include "Engine/Lib/Json/IJsonConverter.h"

class Boss;

/// <summary>
/// 垂直方向からミサイルを連続して放つ
/// </summary>
class BossActionVerticalMissile :
	public AI::BaseTaskNode<Boss> {
public:

	/// <summary>
	/// 垂直ミサイル攻撃の調整パラメータ
	/// </summary>
	struct Parameter : public AOENGINE::IJsonConverter {
		float bulletSpeed = 80.0f;
		float fireRadius = 4.0f;
		int kFireCount = 10;
		float firstSpeedRaito = 0.1f;
		float trakingRaito = 0.05f;

		Parameter() { 
			SetGroupName("BossAction");
			SetName("BossActionVerticalMissile");
		}

		json ToJson(const std::string& id) const override {
			return AOENGINE::JsonBuilder(id)
				.Add("bulletSpeed", bulletSpeed)
				.Add("fireRadius", fireRadius)
				.Add("kFireCount", kFireCount)
				.Add("firstSpeedRaito", firstSpeedRaito)
				.Add("trakingRaito", trakingRaito)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			Convert::fromJson(jsonData, "bulletSpeed", bulletSpeed);
			Convert::fromJson(jsonData, "fireRadius", fireRadius);
			Convert::fromJson(jsonData, "kFireCount", kFireCount);
			Convert::fromJson(jsonData, "firstSpeedRaito", firstSpeedRaito);
			Convert::fromJson(jsonData, "trakingRaito", trakingRaito);
		}

		void Debug_Gui() override;
	};

public:

	BossActionVerticalMissile() = default;
	~BossActionVerticalMissile() override = default;

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
	int fireCount_;

	float angle_;

	bool attackStart_;

};

