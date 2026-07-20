#pragma once
#include <memory>
#include "Game/Scene/BaseScene.h"
// camera
#include "Game/Camera/DebugCamera.h"
#include "Game/Camera/Camera2d.h"
#include "Game/Camera/Camera3d.h"
#include "Game/WorldObject/Skybox.h"
//ui
#include "Game/UI/FadePanel.h"
#include "Game/UI/Guide/GameModeGuide.h"
#include "Game/UI/TitleUIs.h"

// クラスの前方宣言
class ITitleBahavior;

/// <summary>
/// タイトルシーン
/// </summary>
class TitleScene : 
	public BaseScene {
	friend class TitlePushSpaceBehavior;
	friend class TitleGameModeGuideBehavior;
public:

	TitleScene();
	~TitleScene();

	void Finalize() override;
	void Init() override;
	void Update() override;

	void ChangeBehavior(std::unique_ptr<ITitleBahavior> _newBehavior);

	FadePanel* GetFadePanel() const { return fadePanel_.get(); }

private:

	bool putButton_ = false;

	// ------------------- camera ------------------- //
	std::unique_ptr<Camera2d> camera2d_ = nullptr;
	std::unique_ptr<Camera3d> camera3d_ = nullptr;
	std::unique_ptr<DebugCamera> debugCamera_ = nullptr;

	// ------------------- actor ------------------- //
	std::unique_ptr<FadePanel> fadePanel_;

	std::unique_ptr<TitleUIs> titleUIs_;

	std::unique_ptr<ITitleBahavior> titleBehavior_;
};

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ TitleのBehavior
///////////////////////////////////////////////////////////////////////////////////////////////

class ITitleBahavior {
public:
	ITitleBahavior(TitleScene* _host) : host_(_host) {}
	virtual ~ITitleBahavior() = default;

	virtual void Init() = 0;
	virtual void Update() = 0;

protected:
	TitleScene* host_;
	bool isNext_ = false;
};

/// <summary>
/// Spaceh判定用のBehavior
/// </summary>
class TitlePushSpaceBehavior :
	public ITitleBahavior {
public:
	TitlePushSpaceBehavior(TitleScene* _host) :ITitleBahavior(_host) {}
	~TitlePushSpaceBehavior() override = default;
	void Init() override;
	void Update() override;
private:
	bool putButton_ = false;
};

/// <summary>
/// GameMode選択用のBehavior
/// </summary>
class TitleGameModeGuideBehavior :
	public ITitleBahavior {
public:
	TitleGameModeGuideBehavior(TitleScene* _host) :ITitleBahavior(_host) {}
	~TitleGameModeGuideBehavior() override = default;
	void Init() override;
	void Update() override;
private:
	std::unique_ptr<GameModeGuide> gameModeGuide_;
};
