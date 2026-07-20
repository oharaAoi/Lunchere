#pragma once
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Engine/Module/Components/Effect/BaseParticles.h"
#include "Game/Actor/Base/BaseBullet.h"

/// <summary>
/// 追尾をしない爆発弾
/// </summary>
class LauncherBullet :
	public BaseBullet {
public: // データ構造体

	/// <summary>
	/// ランチャー弾の調整パラメータ
	/// </summary>
	struct BulletParam : AOENGINE::IJsonConverter {
		float hitSeVolume = 0.3f;

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

	LauncherBullet() = default;
	~LauncherBullet() override;

public:

	// 初期化
	void Init();
	// 更新
	void Update() override;
	
	/// <summary>
	/// 衝突時の処理
	/// </summary>
	/// <param name="other"></param>
	void OnCollision(AOENGINE::BaseCollider* other);

	/// <summary>
	/// 弾の情報をリセットする
	/// </summary>
	/// <param name="_pos">: 弾の座標</param>
	/// <param name="_velocity">: 速度</param>
	void Reset(const Math::Vector3& _pos, const Math::Vector3& _velocity);

private:
	AOENGINE::BaseParticles* burn_;
	AOENGINE::BaseParticles* smoke_;

	BulletParam param_;
};

