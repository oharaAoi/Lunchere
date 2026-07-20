#pragma once
#include <memory>
#include "Game/Scene/BaseScene.h"
// camera
#include "Game/Camera/DebugCamera.h"
#include "Game/Camera/Camera2d.h"
#include "Game/Camera/Camera3d.h"
#include "Game/Actor/Weapon/Flamethrower.h"

/// <summary>
/// テスト用シーン
/// </summary>
class TestScene 
: public BaseScene {
public:


public:

	TestScene();
	~TestScene() override;

	void Finalize() override;
	void Init() override;
	void Update() override;
	
private:

	// ------------------- camera ------------------- //
	std::unique_ptr<Camera2d> camera2d_ = nullptr;
	std::unique_ptr<Camera3d> camera3d_ = nullptr;
	std::unique_ptr<DebugCamera> debugCamera_ = nullptr;

	std::unique_ptr<Flamethrower> flame_;
};

