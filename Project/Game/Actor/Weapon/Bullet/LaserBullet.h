#pragma once
#include "Engine/Module/Components/WorldTransform.h"
#include "Engine/Module/Components/Collider/BaseCollider.h"
#include "Engine/Module/Components/Collider/LineCollider.h"
#include "Engine/Module/Components/Effect/BaseParticles.h"
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Engine/Utilities/Timer.h"
// game
#include "Game/Actor/Base/BaseBullet.h"
#include "Game/Effects/LaserCylinder.h"

/// <summary>
/// レーザーの弾
/// </summary>
class LaserBullet :
	public BaseBullet {
public: // データ構造体

	/// <summary>
	/// レーザー弾の調整パラメータ
	/// </summary>
	struct LaserParameter : public AOENGINE::IJsonConverter {
		float maxLength = 1000.0f;  // 最大距離
		float lifeTime = 3.0f;
		float fadeTime = 1.0f;
		float shotSeValue = 0.4f;
		float uvScaleY = 100.0f;
		std::string shaderGraphPath = "";

		AOENGINE::Color cylinderColor;

		LaserParameter() { 
			SetGroupName("Bullet");
			SetName("LaserParameter");
		}

		json ToJson(const std::string& id) const override {
			return AOENGINE::JsonBuilder(id)
				.Add("maxLength", maxLength)
				.Add("fadeTime", fadeTime)
				.Add("lifeTime", lifeTime)
				.Add("shotSeValue", shotSeValue)
				.Add("uvScaleY", uvScaleY)
				.Add("shaderGraphPath", shaderGraphPath)
				.Add("cylinderColor", cylinderColor)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			Convert::fromJson(jsonData, "maxLength", maxLength);
			Convert::fromJson(jsonData, "fadeTime", fadeTime);
			Convert::fromJson(jsonData, "lifeTime", lifeTime);
			Convert::fromJson(jsonData, "shotSeValue", shotSeValue);
			Convert::fromJson(jsonData, "uvScaleY", uvScaleY);
			Convert::fromJson(jsonData, "shaderGraphPath", shaderGraphPath);
			Convert::fromJson(jsonData, "cylinderColor", cylinderColor);
		}

		void Debug_Gui() override;
	};

public: // コンストラクタ

	LaserBullet() = default;
	~LaserBullet() override;

public:

	// 初期化
	void Init();
	// 更新
	void Update() override;
	// 編集処理
	void Debug_Gui() override;

	/// <summary>
	/// 衝突時の処理
	/// </summary>
	/// <param name="other"></param>
	void OnCollision(AOENGINE::BaseCollider* _other);

	/// <summary>
	/// 発射した際に行なう初期化処理
	/// </summary>
	/// <param name="_pos"></param>
	/// <param name="_targetPos"></param>
	/// <param name="speed"></param>
	void Reset(const Math::Vector3& _pos, const Math::Vector3& _targetPos, float _speed);

private:

	void Stretch();

	void Fade();

private:

	std::unique_ptr<AOENGINE::WorldTransform> parentTransform_;
	std::unique_ptr<LaserCylinder> laserCylinder_;

	LaserParameter param_;

	bool isShot_ = false;
	bool isFade_ = false;
	Math::Vector3 targetPos_;

	AOENGINE::LineCollider* lineCollider_ = nullptr;

	Math::Vector3 dire_;

	AOENGINE::Timer lifeTimer_;
	AOENGINE::Timer fadeTimer_;
	AOENGINE::Color fadeColor_;

	AOENGINE::BaseParticles* shotEffect_;
	AOENGINE::BaseParticles* shotParticle_;

};

