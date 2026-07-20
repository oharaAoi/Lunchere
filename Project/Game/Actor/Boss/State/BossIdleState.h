#pragma once
#include <Game/State/ICharacterState.h>
#include "Engine/Module/Components/Animation/VectorTween.h"
#include "Engine/Lib/Json/IJsonConverter.h"

class Boss;

/// <summary>
/// 待機時のステート
/// </summary>
class BossIdleState :
	public ICharacterState<Boss> {
public:

	/// <summary>
	/// ボスの待機状態パラメータ
	/// </summary>
	struct Parameter : public AOENGINE::IJsonConverter {
		float start = -0.5f;
		float end = 0.5f;
		float time = 1.5f;

		Parameter() { 
			SetGroupName("BossState");
			SetName("bossActionShotMissile");
		}

		json ToJson(const std::string& id) const override {
			return AOENGINE::JsonBuilder(id)
				.Add("start", start)
				.Add("end", end)
				.Add("time", time)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			Convert::fromJson(jsonData, "start", start);
			Convert::fromJson(jsonData, "end", end);
			Convert::fromJson(jsonData, "time", time);
		}

		void Debug_Gui() override;
	};

public:

	BossIdleState() = default;
	~BossIdleState() = default;

public:

	// 初期化
	void OnStart() override;
	// 更新
	void OnUpdate() override;
	// 終了時
	void OnExit() override;
	// 編集処理
	void Debug_Gui() override;

private:

	AOENGINE::VectorTween<float> floatingTween_;

	Parameter param_;

};

