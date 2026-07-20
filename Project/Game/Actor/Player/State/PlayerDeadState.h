#pragma once
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Engine/Utilities/Timer.h"
#include "Game/State/ICharacterState.h"

class Player;

/// <summary>
/// プレイヤーの死亡状態
/// </summary>
class PlayerDeadState :
	public ICharacterState<Player> {
public: // データ構造体

	/// <summary>
	/// 死亡状態の調整パラメータ
	/// </summary>
	struct Parameter : public AOENGINE::IJsonConverter {
		float knockbackTime = 0.2f;			// ノックバックの時間
		float knockStrength = 10.0f;			// ノックバックの強さ
		float knockDecay = 0.8f;			// ノックバックの減衰

		Parameter() {
			SetGroupName("PlayerState");
			SetName("DeadState");
		}

		json ToJson(const std::string& id) const override {
			return AOENGINE::JsonBuilder(id)
				.Add("knockbackTime", knockbackTime)
				.Add("knockStrength", knockStrength)
				.Add("knockDecay", knockDecay)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			Convert::fromJson(jsonData, "knockbackTime", knockbackTime);
			Convert::fromJson(jsonData, "knockStrength", knockStrength);
			Convert::fromJson(jsonData, "knockDecay", knockDecay);
		}

		void Debug_Gui() override;
	};

public: // コンストラクタ

	PlayerDeadState() = default;
	~PlayerDeadState() = default;

public:

	// 初期化
	void OnStart() override;
	// 更新
	void OnUpdate() override;
	// 終了
	void OnExit() override;
	// 編集
	void Debug_Gui() override;

private:

	// ノックバック処理
	void Knockback();

private:

	Parameter param_;

	AOENGINE::Timer timer_;

	Math::Vector3 velocity_ = CVector3::ZERO;
	Math::Vector3 acceleration_ = CVector3::ZERO;
};

