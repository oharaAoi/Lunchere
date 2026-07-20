#pragma once
#include "Engine/Module/PostEffect/GlitchNoise.h"
#include "Engine/Lib/Json/IJsonConverter.h"
#include <Game/State/ICharacterState.h>

class Player;

/// <summary>
/// プレイヤーのノックバック状態
/// </summary>
class PlayerKnockbackState :
	public ICharacterState<Player> {
public:

	/// <summary>
	/// ノックバック状態の調整パラメータ
	/// </summary>
	struct Parameter : public AOENGINE::IJsonConverter {
		float knockbackTime = 0.2f;			// ノックバックの時間
		float knockStrength = 1.0f;			// ノックバックの強さ
		float knockDecay = 0.8f;			// ノックバックの減衰
		float glitchNoiseTime = 0.2f;		// ポストエフェクトの時間
		float glitchNoiseStrength = 0.2f;	// ポストエフェクトの強さ
		
		Parameter() {
			SetGroupName("PlayerState");
			SetName("knockbackState");
		}

		json ToJson(const std::string& id) const override {
			return AOENGINE::JsonBuilder(id)
				.Add("knockbackTime", knockbackTime)
				.Add("knockStrength", knockStrength)
				.Add("knockDecay", knockDecay)
				.Add("glitchNoiseTime", glitchNoiseTime)
				.Add("glitchNoiseStrength", glitchNoiseStrength)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			Convert::fromJson(jsonData, "knockbackTime", knockbackTime);
			Convert::fromJson(jsonData, "knockStrength", knockStrength);
			Convert::fromJson(jsonData, "knockDecay", knockDecay);
			Convert::fromJson(jsonData, "glitchNoiseTime", glitchNoiseTime);
			Convert::fromJson(jsonData, "glitchNoiseStrength", glitchNoiseStrength);
		}

		void Debug_Gui() override;
	};

public: // コンストラクタ

	PlayerKnockbackState() = default;
	~PlayerKnockbackState() = default;

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

	float timer_ = 0.0f;
	
	Math::Vector3 velocity_;
	Math::Vector3 acceleration_;

	Parameter param_;

	std::shared_ptr<PostEffect::GlitchNoise> glitchNoise_;

};

