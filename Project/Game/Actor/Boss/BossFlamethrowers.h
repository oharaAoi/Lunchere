#pragma once
// c++
#include <memory>
// engine
#include "Engine/Module/Components/Attribute/AttributeGui.h"
#include "Engine/Module/Components/WorldTransform.h"
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Engine/Utilities/Timer.h"
// game
#include "Game/Actor/Weapon/Flamethrower.h"
#include "Game/Manager/BaseBulletManager.h"

enum class BossFlamethrowersType {
	Left,
	Right
};

/// <summary>
/// ボスの火炎放射器を管理するクラス
/// </summary>
class BossFlamethrowers :
	public AOENGINE::AttributeGui {
public:

	/// <summary>
	/// 火炎放射器配置の調整パラメータ
	/// </summary>
	struct Parameter : public AOENGINE::IJsonConverter {
		Math::QuaternionSRT flamethrowerSRT;
		float radius;
		float angle;
		bool clockwise;

		Parameter() {
			SetGroupName("Boss");
			SetName("BossFlamethrower");
		}

		json ToJson(const std::string& id) const override {
			return AOENGINE::JsonBuilder(id)
				.Add("flamethrowerSRT", flamethrowerSRT)
				.Add("radius", radius)
				.Add("angle", angle)
				.Add("clockwise", clockwise)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			Convert::fromJson(jsonData, "flamethrowerSRT", flamethrowerSRT);
			Convert::fromJson(jsonData, "radius", radius);
			Convert::fromJson(jsonData, "angle", angle);
			Convert::fromJson(jsonData, "clockwise", clockwise);
		}

		void Debug_Gui() override;
	};

	/// <summary>
	/// 火炎放射器展開の調整パラメータ
	/// </summary>
	struct DeployFlamethrowerParameter : public AOENGINE::IJsonConverter {
		float deployTime = 1.f;	// 展開時間

		DeployFlamethrowerParameter() {
			SetGroupName("Boss");
			SetName("BossFlamethrowerDeployParameter");
		}

		json ToJson(const std::string& id) const override {
			return AOENGINE::JsonBuilder(id)
				.Add("deployTime", deployTime)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			Convert::fromJson(jsonData, "deployTime", deployTime);
		}

		void Debug_Gui() override;
	};

public:

	BossFlamethrowers() = default;
	~BossFlamethrowers() = default;

public:

	// 初期化
	void Init(AOENGINE::WorldTransform* transform);
	// 更新
	void Update();
	// 編集
	void Debug_Gui() override;

	bool Deploy();

	void Remove();

	void Destroy();

public: // accessor

	const Parameter& GetParameter(BossFlamethrowersType type) const { return param_[(int)type]; }
	void SetParameter(const Parameter& pram, BossFlamethrowersType type) { param_[(int)type] = pram; }

	void SetBulletManager(BaseBulletManager* manager);

	void SetAttackType(FlamethrowerAttackType type);

private:

	static const uint32_t kFlamethrowerCount_ = 2;

	Parameter param_[kFlamethrowerCount_];
	DeployFlamethrowerParameter deployParam_;

	// 武器
	std::unique_ptr<Flamethrower> flamethrowers_[kFlamethrowerCount_];

	AOENGINE::Timer deployTimer_;
};

