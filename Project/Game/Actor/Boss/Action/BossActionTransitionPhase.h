#pragma once
#include <memory>
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Engine/Utilities/Timer.h"
#include "Engine/System/AI/Node/BaseTaskNode.h"
#include "Engine/System/ParticleSystem/Emitter/GpuParticleEmitter.h"
#include "Engine/Module/Components/Effect/BaseParticles.h"
// game
#include "Game/Effects/AttackArmor.h"

class Boss;

/// <summary>
/// 第2形態に遷移するアクション
/// </summary>
class BossActionTransitionPhase :
	public AI::BaseTaskNode<Boss> {
public: // データ構造体

	/// <summary>
	/// ボスのフェーズ移行行動パラメータ
	/// </summary>
	struct Parameter : public AOENGINE::IJsonConverter {
		float chargeTime = 3.0f; // 溜める時間
		float slowTime = 1.0f;
		float slowValue = 0.8f;

		Parameter() {
			SetGroupName("BossAction");
			SetName("BossActionTransitionPhase");
		}

		json ToJson(const std::string& id) const override {
			return AOENGINE::JsonBuilder(id)
				.Add("chargeTime", chargeTime)
				.Add("slowTime", slowTime)
				.Add("slowValue", slowValue)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			Convert::fromJson(jsonData, "chargeTime", chargeTime);
			Convert::fromJson(jsonData, "slowTime", slowTime);
			Convert::fromJson(jsonData, "slowValue", slowValue);
		}

		void Debug_Gui() override;
	};

public: // コンストラクタ

	BossActionTransitionPhase();
	~BossActionTransitionPhase() override = default;

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
	AOENGINE::GpuParticleEmitter* chargeParticle_;
	AOENGINE::BaseParticles* chargeLine_;

	AOENGINE::Timer slowTimer_;

	std::unique_ptr<AttackArmor> attackArmor_ = nullptr;
};

