#pragma once
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Engine/System/AI/Node/BaseTaskNode.h"

class Boss;

/// <summary>
/// バリアを展開する
/// </summary>
class BossActionDeployArmor :
	public AI::BaseTaskNode<Boss> {
public:

	/// <summary>
	/// ボスのアーマー展開行動パラメータ
	/// </summary>
	struct Parameter : public AOENGINE::IJsonConverter {
		float deployTime = 3.0f;
		float randShakeValue = 2.0f;

		Parameter() {
			SetGroupName("BossAction");
			SetName("bossActionDeployArmor"); 
		}
		
		json ToJson(const std::string& id) const override {
			return AOENGINE::JsonBuilder(id)
				.Add("deployTime", deployTime)
				.Add("randShakeValue", randShakeValue)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			Convert::fromJson(jsonData, "deployTime", deployTime);
			Convert::fromJson(jsonData, "randShakeValue", randShakeValue);
		}

		void Debug_Gui() override;
	};
public:

	BossActionDeployArmor();
	~BossActionDeployArmor() override = default;

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
	bool isDeploy_ = false;

};

