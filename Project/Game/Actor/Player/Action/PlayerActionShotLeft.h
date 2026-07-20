#pragma once
#include <memory>
#include <functional>
// Engine
#include "Engine/System/Input/Input.h"
#include "Engine/Lib/Json/IJsonConverter.h"
// Game
#include "Game/Actor/Base/BaseAction.h"
#include "Game/Actor/Weapon/BaseWeapon.h"
#include "Game/Camera/Animation/ICameraAnimation.h"

class Player;

/// <summary>
/// Playerの左手の攻撃
/// </summary>
class PlayerActionShotLeft :
	public BaseAction<Player> {
public:

	/// <summary>
	/// 左手射撃行動の調整パラメータ
	/// </summary>
	struct Parameter : public AOENGINE::IJsonConverter {
		float animationTime = 0.2f;
		float cameraShakeTime = 0.2f;
		float cameraShakeStrength = 1.0f;

		Parameter() {
			SetGroupName("PlayerAction");
			SetName("actionShotLeft");
		}

		json ToJson(const std::string& id) const override {
			return AOENGINE::JsonBuilder(id)
				.Add("animationTime", animationTime)
				.Add("cameraShakeTime", cameraShakeTime)
				.Add("cameraShakeStrength", cameraShakeStrength)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			Convert::fromJson(jsonData, "animationTime", animationTime);
			Convert::fromJson(jsonData, "cameraShakeTime", cameraShakeTime);
			Convert::fromJson(jsonData, "cameraShakeStrength", cameraShakeStrength);
		}

		void Debug_Gui() override;
	};

public: // コンストラクタ

	PlayerActionShotLeft() = default;
	~PlayerActionShotLeft()  override {};

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

	// 攻撃
	void Shot();

	// 準備関数
	void StartUp();

	// 反動
	void Recoil();

private:

	float notShotTime_ = 1.0f;	// 弾を放っていない時間を計測するよう
	float notShotTimer_ = 0.0f;
	bool isFinish_ = true;

	AOENGINE::Input* pInput_;
	ICameraAnimation* pCameraAnimation_ = nullptr;
	BaseWeapon* pWeapon_ = nullptr;

	Parameter param_;

	std::function<void()> action_;

};

