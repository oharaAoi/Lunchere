#pragma once
#include "Game/Actor/Base/BaseBullet.h"
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Engine/Module/Components/Effect/BaseParticles.h"

/// <summary>
/// ロケットミサイル弾
/// </summary>
class RocketBullet :
	public BaseBullet {
public: // データ構造体

	/// <summary>
	/// ロケット弾の調整パラメータ
	/// </summary>
	struct BulletParam : AOENGINE::IJsonConverter {
		float hitSeVolume = 0.3f; // hit時のSEの音量

		BulletParam() {
			SetGroupName("Bullet");
			SetName("launcherBullet");
		}

		json ToJson(const std::string& id) const override {
			return AOENGINE::JsonBuilder(id)
				.Add("hitSeVolume", hitSeVolume)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			Convert::fromJson(jsonData, "hitSeVolume", hitSeVolume);
		}

		void Debug_Gui() override;
	};

public: // コンストラクタ

	RocketBullet() = default;
	~RocketBullet() override;

public:

	// 初期化
	void Init();
	// 更新
	void Update() override;
	
	/// <summary>
	/// 衝突判定
	/// </summary>
	/// <param name="other">: 衝突collider</param>
	void OnCollision(AOENGINE::BaseCollider* other);

	/// <summary>
	/// 弾の情報をリセットする
	/// </summary>
	/// <param name="pos">: 弾の座標</param>
	/// <param name="target">: 目標の座標</param>
	/// <param name="bulletSpeed">: 弾の速度</param>
	void Reset(const Math::Vector3& _pos, const Math::Vector3& _target, float _bulletSpeed, float _trackingLength, float _trackingTime, float _trackingRaito, bool isLockOn);

private:

	void Tracking();

private:

	// 追尾に関する変数
	float trackingLength_;		// 長さ
	float trackingTimer_;		// 計測時間
	float trackingTime_ = 1.f;	// 追尾するまでの時間
	float trackingRaito_ = 0.8f;
	bool finishTracking_;		// 追尾を終了するか
	bool isLockOn_ = true;
	bool isCalcDirection_ = false;

	BulletParam param_;

	AOENGINE::BaseParticles* burn_;
	AOENGINE::BaseParticles* smoke_;
	Math::Vector3 direction_;
};

