#pragma once
#include <memory>
#include "Engine/Lib/Math/Quaternion.h"
#include "Engine/System/AI/Node/BaseTaskNode.h"
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Engine/Utilities/Timer.h"

class Boss;

/// <summary>
/// 2弾式のミサイル
/// </summary>
class BossActionDualStageMissile :
	public AI::BaseTaskNode<Boss> {
public: // データ構造体

	/// <summary>
	/// 二段ミサイル攻撃の調整パラメータ
	/// </summary>
	struct Parameter : public AOENGINE::IJsonConverter {
		float bulletSpeed = 90.0f;	// 弾速
		float takeDamage = 30.0f;	// 与えるダメージ
		uint32_t fireNum = 14;		// 発射する数
		float firstSpeedRaito = 0.1f;	// 初期速度の割合
		float trakingRaito = 0.05f;		// 追従の割合

		float spreadAngleDeg = 60.0f;	// 拡散する角度
		float secondPhaseTime = 0.5f;	// 2発目までの時間
		float lineOffset = 1.0f;		// 2列間の距離

		Parameter() { 
			SetGroupName("BossAction"); 
			SetName("BossActionDualStageMissile"); 
		}

		json ToJson(const std::string& id) const override {
			return AOENGINE::JsonBuilder(id)
				.Add("bulletSpeed", bulletSpeed)
				.Add("takeDamage", takeDamage)
				.Add("fireNum", fireNum)
				.Add("firstSpeedRaito", firstSpeedRaito)
				.Add("trakingRaito", trakingRaito)
				.Add("spreadAngleDeg", spreadAngleDeg)
				.Add("secondPhaseTime", secondPhaseTime)
				.Add("lineOffset", lineOffset)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			Convert::fromJson(jsonData, "bulletSpeed", bulletSpeed);
			Convert::fromJson(jsonData, "takeDamage", takeDamage);
			Convert::fromJson(jsonData, "fireNum", fireNum);
			Convert::fromJson(jsonData, "firstSpeedRaito", firstSpeedRaito);
			Convert::fromJson(jsonData, "trakingRaito", trakingRaito);
			Convert::fromJson(jsonData, "spreadAngleDeg", spreadAngleDeg);
			Convert::fromJson(jsonData, "secondPhaseTime", secondPhaseTime);
			Convert::fromJson(jsonData, "lineOffset", lineOffset);
		}

		void Debug_Gui() override;
	};

public: // コンストラクタ

	BossActionDualStageMissile() = default;
	~BossActionDualStageMissile() = default;

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

	/// <summary>
	/// 発射
	/// </summary>
	void Shot();

	/// <summary>
	/// Playerの方向を向かせる処理
	/// </summary>
	void LookPlayer();

private:

	Parameter param_;

	// 弾を打ち終わったかのフラグ
	bool isFinishShot_ = false;

	// 2発目かどうか
	bool isSecondPhase_ = false;

	AOENGINE::Timer secondPhaseTimer_;
};

