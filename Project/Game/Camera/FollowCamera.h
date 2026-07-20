#pragma once
#include <string>
#include <unordered_map>
#include "Engine/Module/Components/Attribute/AttributeGui.h"
#include "Engine/Module/PostEffect/Grayscale.h"
#include "Engine/Module/PostEffect/Vignette.h"
#include "Game/Camera/BaseCamera.h"
#include "Game/UI/Reticle.h"
#include "Game/Camera/Animation/ICameraAnimation.h"

class Player;

/// <summary>
/// Playerに追従するCamera
/// </summary>
class FollowCamera :
	public BaseCamera,
	public AOENGINE::AttributeGui {
public:

	/// <summary>
	/// 追従カメラの設定パラメータ
	/// </summary>
	struct CameraParameter : public AOENGINE::IJsonConverter {
		float distance = 20.0f;	// カメラの距離
		float rotateDelta = 0.04f;
		Math::Vector3 offset = { 0,2.5f,0.0f };
		float complement = 0.5f; // カメラ移動の際の補完

		float limitMinY;	// カメラ回転の限界(最小)
		float limitMaxY;	// カメラ回転の限界(最大)

		float followHeight;	// playerより少し上に配置するための距離

		float smoothTime = 0.1f;	// 追従の速度
		float maxSpeed = 10 ^ 8;	// 追従の最大速度

		int easingIndex;

		CameraParameter() {
			SetGroupName("Camera");
			SetName("FollowCamera");
		}

		json ToJson(const std::string& id) const override {
			return AOENGINE::JsonBuilder(id)
				.Add("distance", distance)
				.Add("rotateDelta", rotateDelta)
				.Add("offset", offset)
				.Add("complement", complement)
				.Add("limitMinY", limitMinY)
				.Add("limitMaxY", limitMaxY)
				.Add("smoothTime", smoothTime)
				.Add("maxSpeed", maxSpeed)
				.Add("easingIndex", easingIndex)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			Convert::fromJson(jsonData, "distance", distance);
			Convert::fromJson(jsonData, "rotateDelta", rotateDelta);
			Convert::fromJson(jsonData, "offset", offset);
			Convert::fromJson(jsonData, "complement", complement);
			Convert::fromJson(jsonData, "limitMinY", limitMinY);
			Convert::fromJson(jsonData, "limitMaxY", limitMaxY);
			Convert::fromJson(jsonData, "smoothTime", smoothTime);
			Convert::fromJson(jsonData, "maxSpeed", maxSpeed);
			Convert::fromJson(jsonData, "easingIndex", easingIndex);
		}

		void Debug_Gui() override;
	};

	/// <summary>
	/// カメラ演出の設定パラメータ
	/// </summary>
	struct AnimationParameter : public AOENGINE::IJsonConverter {
		Math::Vector3 firstOffset;
		Math::Vector3 targetOffset;
		float moveTime;
		int easingIndex;
		AOENGINE::Color scaleColor;
		float vignettePower;

		AnimationParameter() {
			SetGroupName("Camera");
			SetName("AnimationParameter");
		}

		json ToJson(const std::string& id) const override {
			return AOENGINE::JsonBuilder(id)
				.Add("firstOffset", firstOffset)
				.Add("moveTime", moveTime)
				.Add("easingIndex", easingIndex)
				.Add("scaleColor", scaleColor)
				.Add("vignettePower", vignettePower)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			Convert::fromJson(jsonData, "firstOffset", firstOffset);
			Convert::fromJson(jsonData, "moveTime", moveTime);
			Convert::fromJson(jsonData, "easingIndex", easingIndex);
			Convert::fromJson(jsonData, "scaleColor", scaleColor);
			Convert::fromJson(jsonData, "vignettePower", vignettePower);
		}

		void Debug_Gui() override;
	};

public:

	FollowCamera();
	~FollowCamera() override = default;

public:

	// 終了処理
	void Finalize() override;
	// 初期化
	void Init() override;
	// 更新
	void Update() override;
	// 編集
	void Debug_Gui() override;

private:	// private method

	/// <summary>
	/// Stick入力を受け取る
	/// </summary>
	void InputStick();

	/// <summary>
	/// Cameraを回転させる
	/// </summary>
	void RotateCamera();

	/// <summary>
	/// カメラを動かす
	/// </summary>
	/// <param name="target"></param>
	void MoveCamera(const Math::Vector3& target);

	/// <summary>
	/// カメラをシェイクさせる
	/// </summary>
	void Shake();

	/// <summary>
	/// ゲーム開始時のカメラの動き
	/// </summary>
	void FirstCameraMove();

public:		// accessor method

	ICameraAnimation* GetCameraAnimation(const std::string& _name) const { return animationMap_.at(_name).get(); }

	void SetOffset(const Math::Vector3& _offset) { followCamera_.offset = _offset; }
	const Math::Vector3& GetOffset() const { return followCamera_.offset; }

	void SetOffsetZ(float _offsetZ) { followCamera_.offset.z = _offsetZ; }

	const Math::Vector3& GetInitOffset() const { return initFollowCamera_.offset; }

	void SetShake(float time, float strength);

	void SetTarget(Player* _target);

	void SetReticle(Reticle* reticle) { pReticle_ = reticle; }

	Math::Quaternion GetAngleX();

	void SetAngleZ(float _angle) { angle_.z = _angle; }

private:

	// 他クラス ------------------------------------------------

	Player* pTarget_ = nullptr;

	Reticle* pReticle_ = nullptr;

	// Parameter ------------------------------------------------

	Math::QuaternionSRT pivotSRT_;

	CameraParameter followCamera_;
	CameraParameter initFollowCamera_;

	Math::Vector3 angle_ = {};

	const float kDeadZone_ = 0.4f;
	Math::Vector2 stick_;

	Math::Vector3 velocity_;

	float rotateLength_;

	Math::Vector3 prePosition_;

	// Shake ------------------------------------------------
	float shakeTimer_ = 1.0f;
	float shakeTime_ = 1.0f;
	float shakeStrength_;

	// Animation ------------------------------------------------
	AnimationParameter animationParam_;
	float animationTimer_;
	bool isAnimationFinish_;
	std::shared_ptr<PostEffect::Grayscale> grayscale_;
	std::shared_ptr<PostEffect::Vignette> vignette_;

	std::unordered_map<std::string, std::unique_ptr<ICameraAnimation>> animationMap_;

};

