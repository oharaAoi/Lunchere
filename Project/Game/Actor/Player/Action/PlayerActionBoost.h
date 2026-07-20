#pragma once
#include <functional>
// Engine
#include "Engine/System/Input/Input.h"
#include "Engine/Module/Components/WorldTransform.h"
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Engine/Module/PostEffect/RadialBlur.h"
#include "Engine/Module/Components/Physics/Rigidbody.h"
// Game
#include "Game/Actor/Base/BaseAction.h"
#include "Game/Camera/Animation/ICameraAnimation.h"

// 前方宣言
class Player;

/// <summary>
/// Boostを行うクラス
/// </summary>
class PlayerActionBoost :
	public BaseAction<Player> {
public:

	/// <summary>
	/// ブースト行動の調整パラメータ
	/// </summary>
	struct Parameter : public AOENGINE::IJsonConverter {
		float chargeTime;			// 溜める時間
	
		float boostForce;			// ブーストの力
		float stopForce;			// 止める力

		float bluerStrength;		// ブラーの強さ
		float bluerStartTime;		// ブラーがかかるまでの時間
		float bluerStopTime;		// ブラーが止まるまでの時間

		float consumeEnergy;		// 消費エネルギー

		float overTakingThreshold;	// 追い越し防止閾値
		float stopThreshold;		// 止まる閾値

		float toQuickBoostThreshold = 0.8f; // quickBoostに遷移する際の閾値

		Parameter() { 
			SetGroupName("PlayerAction");
			SetName("ActionBoost"); 
		}

		json ToJson(const std::string& id) const override {
			return AOENGINE::JsonBuilder(id)
				.Add("chargeTime", chargeTime)
				.Add("boostForce", boostForce)
				.Add("stopForce", stopForce)
				.Add("bluerStrength", bluerStrength)
				.Add("bluerStartTime", bluerStartTime)
				.Add("bluerStopTime", bluerStopTime)
				.Add("consumeEnergy", consumeEnergy)
				.Add("overTakingThreshold", overTakingThreshold)
				.Add("stopThreshold", stopThreshold)
				.Add("toQuickBoostThreshold", toQuickBoostThreshold)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			Convert::fromJson(jsonData, "chargeTime", chargeTime);
			Convert::fromJson(jsonData, "boostForce", boostForce);
			Convert::fromJson(jsonData, "stopForce", stopForce);
			Convert::fromJson(jsonData, "bluerStrength", bluerStrength);
			Convert::fromJson(jsonData, "bluerStartTime", bluerStartTime);
			Convert::fromJson(jsonData, "bluerStopTime", bluerStopTime);
			Convert::fromJson(jsonData, "consumeEnergy", consumeEnergy);
			Convert::fromJson(jsonData, "overTakingThreshold", overTakingThreshold);
			Convert::fromJson(jsonData, "stopThreshold", stopThreshold);
			Convert::fromJson(jsonData, "toQuickBoostThreshold", toQuickBoostThreshold);
		}

		void Debug_Gui() override;
	};

public:

	PlayerActionBoost() = default;
	~PlayerActionBoost() override;

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

	/// <summary>
	/// ブーストの溜めを行う
	/// </summary>
	void BoostCharge();

	/// <summary>
	/// ブーストの中止
	/// </summary>
	void BoostStop();

	/// <summary>
	/// 中止確認
	/// </summary>
	/// <returns></returns>
	bool CheckStop();

private:

	// 他クラス ------------------------------------------------
	AOENGINE::Input* pInput_;
	AOENGINE::Rigidbody* pRigidbody_ = nullptr;
	AOENGINE::WorldTransform* pOwnerTransform_ = nullptr;
	ICameraAnimation* pCameraAnimation_ = nullptr;

	// State/Parameter ------------------------------------------------
	Parameter param_;
	Parameter initialPram_;
	
	Math::Vector3 acceleration_ = { 0.0f, 0.0f, 0.0f };
	Math::Vector3 direction_;

	// boost
	std::function<void()> mainAction_;
	bool isStop_ = false;
	bool isSlow_ = false;
	
	// time
	float timer_;

	// input
	const float kDeadZone_ = 0.1f;
	Math::Vector2 stick_;

	std::shared_ptr<PostEffect::RadialBlur> blur_;

};

