#pragma once
#include "Engine/System/Input/Input.h"
#include "Engine/Lib/Math/Curve.h"
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Engine/Module/Components/WorldTransform.h"
#include "Engine/Module/Components/Physics/Rigidbody.h"
#include "Engine/Module/Components/Effect/BaseParticles.h"
#include "Engine/Module/PostEffect/RadialBlur.h"
#include "Game/Actor/Base/BaseAction.h"

// 前方宣言
class Player;

/// <summary>
/// クイックブーストを行うクラス
/// </summary>
class PlayerActionQuickBoost :
	public BaseAction<Player> {
public:

	/// <summary>
	/// クイックブースト行動の調整パラメータ
	/// </summary>
	struct Parameter : public AOENGINE::IJsonConverter {
		float boostForce = 10.0f;		// boostの強さ
		
		float boostEnergy = 20.0f;		// 消費エネルギー

		float cameraShakeTime = 0.2f;
		float cameraShakeStrength = 0.1f;

		float decelerationTime = 1.0f;		// 減速までの時間

		float blurTime = 0.2f;
		float blurStrength = 0.2f;

		float vibrateTime = 0.2f;
		float vibrateStrength = 1.0f;

		Math::Curve decelerationCurve;	// 減速の際のカーブ

		Parameter() { SetName("ActionQuickBoost"); }

		json ToJson(const std::string& id) const override {
			json curveJson = decelerationCurve.ToJson();
			return AOENGINE::JsonBuilder(id)
				.Add("boostForce", boostForce)
				.Add("boostEnergy", boostEnergy)
				.Add("cameraShakeTime", cameraShakeTime)
				.Add("cameraShakeStrength", cameraShakeStrength)
				.Add("blurTime", blurTime)
				.Add("blurStrength", blurStrength)
				.Add("decelerationTime", decelerationTime)
				.Add("vibrateTime", vibrateTime)
				.Add("vibrateStrength", vibrateStrength)
				.Add("decelerationCurve", curveJson)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			Convert::fromJson(jsonData, "boostForce", boostForce);
			Convert::fromJson(jsonData, "boostEnergy", boostEnergy);
			Convert::fromJson(jsonData, "cameraShakeTime", cameraShakeTime);
			Convert::fromJson(jsonData, "cameraShakeStrength", cameraShakeStrength);
			Convert::fromJson(jsonData, "blurTime", blurTime);
			Convert::fromJson(jsonData, "blurStrength", blurStrength);
			Convert::fromJson(jsonData, "decelerationTime", decelerationTime);
			Convert::fromJson(jsonData, "vibrateTime", vibrateTime);
			Convert::fromJson(jsonData, "vibrateStrength", vibrateStrength);
			decelerationCurve.FromJson(jsonData, "decelerationCurve");
		}

		void Debug_Gui() override;
	};

public:

	PlayerActionQuickBoost() = default;
	~PlayerActionQuickBoost() override {};

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

private:

	/// <summary>
	/// ブーストを行う
	/// </summary>
	void Boost();

private:

	// 他クラス ------------------------------------------------
	AOENGINE::Input* pInput_;
	AOENGINE::WorldTransform* pOwnerTransform_ = nullptr;
	std::shared_ptr<PostEffect::RadialBlur> pRadialBlur_;

	// State/Parameter ------------------------------------------------
	Math::Vector3 acceleration_ = { 0.0f, 0.0f, 0.0f };
	Math::Vector3 direction_;

	const float kDeadZone_ = 0.1f;
	Math::Vector2 stick_;

	Parameter param_;
	Parameter initParam_;

	AOENGINE::Rigidbody* pRigidBody_;

	size_t boostClassId_;

};

