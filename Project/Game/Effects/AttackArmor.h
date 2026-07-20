#pragma once
#include <memory>
// Engine
#include "Engine/Module/Components/GameObject/BaseEntity.h"
#include "Engine/System/ShaderGraph/ShaderGraph.h"
#include "Engine/System/ParticleSystem/Emitter/GpuParticleEmitter.h"
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Engine/Utilities/Timer.h"

/// <summary>
/// 攻撃に使用するアーマー
/// </summary>
class AttackArmor :
	public AOENGINE::BaseEntity {
public:

	/// <summary>
	/// アタックアーマーの設定パラメータ
	/// </summary>
	struct Parameter : public AOENGINE::IJsonConverter {
		std::string shaderGraphPath = "";			// shaderGraphのパス
		Math::Vector3 uvScale = CVector3::UNIT;		// uvのスケール
		Math::Vector3 upScale = CVector3::UNIT;		// 目標のスケール
		float upScaleTargetTime;					// スケールを大きくする時間
		float disappearTargetTime;					// 消える時間

		float cameraShakeStrength = 4;				// カメラシェイクの強さ
		float cameraShakeTime = 1;						// カメラシェイクの時間

		Parameter() {
			SetGroupName("Effect");
			SetName("AttackArmorParameter");
		}

		json ToJson(const std::string& id) const override {
			return AOENGINE::JsonBuilder(id)
				.Add("uvScale", uvScale)
				.Add("upScale", upScale)
				.Add("upScaleTargetTime", upScaleTargetTime)
				.Add("disappearTargetTime", disappearTargetTime)
				.Add("shaderGraphPath", shaderGraphPath)
				.Add("cameraShakeStrength", cameraShakeStrength)
				.Add("cameraShakeTime", cameraShakeTime)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			Convert::fromJson(jsonData, "uvScale", uvScale);
			Convert::fromJson(jsonData, "upScale", upScale);
			Convert::fromJson(jsonData, "upScaleTargetTime", upScaleTargetTime);
			Convert::fromJson(jsonData, "disappearTargetTime", disappearTargetTime);
			Convert::fromJson(jsonData, "shaderGraphPath", shaderGraphPath);
			Convert::fromJson(jsonData, "cameraShakeStrength", cameraShakeStrength);
			Convert::fromJson(jsonData, "cameraShakeTime", cameraShakeTime);
		}

		void Debug_Gui() override;
	};

public: // コンストラクタ

	AttackArmor() = default;
	virtual ~AttackArmor();

public:

	// 初期化
	void Init();

	// 更新
	void Update();

	// 編集
	virtual void Debug_Gui() override;

	void Start(const Math::Vector3& _pos);

private:

	// スケールアップ
	void ScaleUp();

	// 消える
	void Disappear();

public:

	void SetIsStart(bool _isStart) { isStart_ = _isStart; }
	bool GetIsStart() const { return isStart_; }

	bool GetIsFinish() const { return isFinish_; }
	bool GetIsDisappear() const { return isDisappear_; }

	float GetCameraShakeStrength() const { return parameter_.cameraShakeStrength; }
	float GetCameraShakeTime() const { return parameter_.cameraShakeTime; }

private:

	std::unique_ptr<AOENGINE::ShaderGraph> shaderGraph_;

	AOENGINE::GpuParticleEmitter* particle_;

	AOENGINE::BaseCollider* collider_;

	Parameter parameter_;
	AOENGINE::Timer upScaleTimer_;
	AOENGINE::Timer disappearTimer_;

	bool isStart_ = false;
	bool isDisappear_ = false;

	bool isFinish_ = false;

};
