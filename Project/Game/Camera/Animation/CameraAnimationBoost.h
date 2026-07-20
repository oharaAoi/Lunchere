#pragma once
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Game/Camera/Animation/ICameraAnimation.h"

class FollowCamera;

/// <summary>
/// Boost時のカメラアニメーション
/// </summary>
class CameraAnimationBoost :
	public ICameraAnimation {
public: // データ構造体

	/// <summary>
	/// ブースト時カメラ演出のパラメータ
	/// </summary>
	struct BoostAnimationParam : public AOENGINE::IJsonConverter {
		bool isExecute = false;
		bool isApproach = false;
		float timer;
		// editor, dragFloat3, 0.1
		Math::Vector3 offset;
		// editor, dragFloat, 0.1
		float time;

		BoostAnimationParam() {
			SetGroupName("Camera");
			SetName("BoostAnimation");
		};

		json ToJson(const std::string& id) const override {
			return AOENGINE::JsonBuilder(id)
				.Add("offset", offset)
				.Add("time", time)
				.Build(); 
		}

		void FromJson(const json& jsonData) override {
			Convert::fromJson(jsonData, "offset", offset);
			Convert::fromJson(jsonData, "time", time);
		}

		void Debug_Gui() override;
	};

public: // コンストラクタ

	CameraAnimationBoost() = default;
	~CameraAnimationBoost() override = default;

public:

	// 初期化
	void Init() override;
	// 更新
	void Update() override;
	// 実行処理を呼ぶ
	void CallExecute(bool _isRevers) override;
	// 編集処理
	void Debug_Gui() override;

private:

	/// <summary>
	/// Boost時のカメラの寄り
	/// </summary>
	void BoostAnimation();

private:

	BoostAnimationParam param_;
	Math::Vector3 cameraOffset_;

};

