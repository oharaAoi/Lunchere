#pragma once
#include "Engine/Module/Components/Physics/Rigidbody.h"
#include "Game/Actor/Base/BaseAction.h"
#include <Lib/Json/IJsonConverter.h>

// 前方宣言
class Player;

/// <summary>
/// 移動アクション
/// </summary>
class PlayerActionMove :
	public BaseAction<Player> {
public:

	/// <summary>
	/// 移動行動の調整パラメータ
	/// </summary>
	struct Parameter : public AOENGINE::IJsonConverter {
		float speed;				// 歩く速度
		float boostSpeed;			// ブーストの速度
		float maxSpeed;
		float rotateT = 0.5f;		// 回転の速度
		float decayRate = 2.0f;		// 減速の際の倍率
		float turnAroundThreshold;	// 回転をする際の
		float turnSpeed = 0.3f;	// 回転をする際の
		float animationTime = 0.1f;
		float accel = 20.0f;
		float decel = 30.0f;

		Parameter() { SetName("ActionMove"); }

		json ToJson(const std::string& id) const override {
			return AOENGINE::JsonBuilder(id)
				.Add("speed", speed)
				.Add("boostSpeed", boostSpeed)
				.Add("maxSpeed", maxSpeed)
				.Add("rotateT", rotateT)
				.Add("decayRate", decayRate)
				.Add("animationTime", animationTime)
				.Add("turnAroundThreshold", turnAroundThreshold)
				.Add("turnSpeed", turnSpeed)
				.Add("accel", accel)
				.Add("decel", decel)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			Convert::fromJson(jsonData, "speed", speed);
			Convert::fromJson(jsonData, "boostSpeed", boostSpeed);
			Convert::fromJson(jsonData, "maxSpeed", maxSpeed);
			Convert::fromJson(jsonData, "rotateT", rotateT);
			Convert::fromJson(jsonData, "decayRate", decayRate);
			Convert::fromJson(jsonData, "animationTime", animationTime);
			Convert::fromJson(jsonData, "turnAroundThreshold", turnAroundThreshold);
			Convert::fromJson(jsonData, "turnSpeed", turnSpeed);
			Convert::fromJson(jsonData, "accel", accel);
			Convert::fromJson(jsonData, "decel", decel);
		}

		void Debug_Gui() override;
	};

public:

	PlayerActionMove() = default;
	~PlayerActionMove() override {};

public:

	// ビルド処理
	void Build() override;
	// 初期化
	void OnStart() override;
	// 更新
	void OnUpdate() override;
	// 終了処理
	void OnEnd() override;
	// 次のアクションの判定
	void CheckNextAction() override;
	// actionの入力判定
	bool IsInput() override;
	// 編集処理
	void Debug_Gui() override;

private:	// action

	/// <summary>
	/// 動く処理
	/// </summary>
	void Move();

private:

	// Parameter -------------------------------

	AOENGINE::Rigidbody* pRigidbody_ = nullptr;

	const float kDeadZone_ = 0.1f;
	Math::Vector2 inputStick_;
	Math::Vector2 preInputStick_;

	Math::Vector3 accel_;
	Math::Vector3 velocity_;
	Math::Vector3 preVelocity_;

	Parameter param_;

	bool isTurnAround_;

	bool preBoost_;
};

