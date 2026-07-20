#pragma once
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Engine/Lib/Math/Vector3.h"
#include "Engine/Lib/Color.h"

/// <summary>
/// プレイヤーの調整パラメータ
/// </summary>
struct PlayerParameter : public AOENGINE::IJsonConverter {
	float health;				// hp
	float postureStability;		// 姿勢安定
	float psRecoveryTime = 3.0f;// 姿勢安定回復時間
	float psRecoveryValue = 3.0f;// 姿勢安定回復量
	float bodyWeight = 1.0f;	// 機体の重さ

	float energy = 1.f;	// EN出力
	float energyRecoveryAmount = 1.f;	// EN回復量(m/s)
	float energyRecoveryCoolTime;	// EN回復までのクールタイム

	float legColliderRadius = 0.5f;
	float legColliderPosY = -0.1f;

	float windDrag;	// 空気抵抗量

	float inclineStrength = 0.1f;		// 傾きの強さ
	float inclineReactionRate = 5.0f;	// 傾きの反応速度
	float inclineThreshold = 10.0f;

	float invincibleTime = 1.0f;				// 無敵時間

	float pinchOfPercentage = 0.3f;			// ピンチの割合
	float pinchVignettePower;
	AOENGINE::Color pinchVignetteColor;

	Math::Vector3 cameraOffset = CVector3::ZERO;
	Math::Vector3 translateOffset = CVector3::ZERO;

	PlayerParameter() {
		SetGroupName("Player");
		SetName("playerParameter");
	}

	json ToJson(const std::string& id) const override {
		return AOENGINE::JsonBuilder(id)
			.Add("health", health)
			.Add("postureStability", postureStability)
			.Add("psRecoveryTime", psRecoveryTime)
			.Add("psRecoveryValue", psRecoveryValue)
			.Add("bodyWeight", bodyWeight)
			.Add("energy", energy)
			.Add("energyRecoveryAmount", energyRecoveryAmount)
			.Add("energyRecoveryCoolTime", energyRecoveryCoolTime)
			.Add("legColliderRadius", legColliderRadius)
			.Add("legColliderPosY", legColliderPosY)
			.Add("windDrag", windDrag)
			.Add("inclineStrength", inclineStrength)
			.Add("inclineReactionRate", inclineReactionRate)
			.Add("inclineThreshold", inclineThreshold)
			.Add("cameraOffset", cameraOffset)
			.Add("translateOffset", translateOffset)
			.Add("pinchOfPercentage", pinchOfPercentage)
			.Add("pinchVignettePower", pinchVignettePower)
			.Add("pinchVignetteColor", pinchVignetteColor)
			.Add("invincibleTime", invincibleTime)
			.Build();
	}

	void FromJson(const json& jsonData) override {
		Convert::fromJson(jsonData, "health", health);
		Convert::fromJson(jsonData, "postureStability", postureStability);
		Convert::fromJson(jsonData, "psRecoveryTime", psRecoveryTime);
		Convert::fromJson(jsonData, "psRecoveryValue", psRecoveryValue);
		Convert::fromJson(jsonData, "bodyWeight", bodyWeight);
		Convert::fromJson(jsonData, "energy", energy);
		Convert::fromJson(jsonData, "energyRecoveryAmount", energyRecoveryAmount);
		Convert::fromJson(jsonData, "energyRecoveryCoolTime", energyRecoveryCoolTime);
		Convert::fromJson(jsonData, "legColliderRadius", legColliderRadius);
		Convert::fromJson(jsonData, "legColliderPosY", legColliderPosY);
		Convert::fromJson(jsonData, "windDrag", windDrag);
		Convert::fromJson(jsonData, "inclineStrength", inclineStrength);
		Convert::fromJson(jsonData, "inclineReactionRate", inclineReactionRate);
		Convert::fromJson(jsonData, "inclineThreshold", inclineThreshold);
		Convert::fromJson(jsonData, "cameraOffset", cameraOffset);
		Convert::fromJson(jsonData, "translateOffset", translateOffset);
		Convert::fromJson(jsonData, "pinchOfPercentage", pinchOfPercentage);
		Convert::fromJson(jsonData, "pinchVignettePower", pinchVignettePower);
		Convert::fromJson(jsonData, "pinchVignetteColor", pinchVignetteColor);
		Convert::fromJson(jsonData, "invincibleTime", invincibleTime);
	}

	void Debug_Gui() override;
};

