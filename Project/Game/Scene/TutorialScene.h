#pragma once
#include <memory>
#include "Game/Scene/BaseScene.h"
// camera
#include "Game/Camera/FollowCamera.h"
#include "Game/Camera/DebugCamera.h"
#include "Game/Camera/Camera2d.h"
// sprite
#include "Game/UI/CanvasUI.h"
#include "Game/UI/FadePanel.h"
#include "Game/UI/Guide/TutorialMissionGauge.h"
// manager
#include "Engine/System/Manager/CollisionManager.h"
#include "Game/CallBacks/GameCallBacksManager.h"
// scene
#include "Engine/Render/SceneRenderer.h"
// actor
#include "Game/WorldObject/Skybox.h"
#include "Game/Actor/Player/PlayerManager.h"
#include "Game/Actor/Enemy/EnemyManager.h"

class ITutorialBehavior;

class TutorialScene :
	public BaseScene {
	friend class TutorialMoveBehavior;
	friend class TutorialJumpBehavior;
	friend class TutorialBoostBehavior;
	friend class TutorialLockOnBehavior;
	friend class TutorialAttackBehavior;
	friend class TutorialFreeModeBehavior;
public:

	TutorialScene();
	~TutorialScene();

	void Finalize() override;
	void Init() override;
	void Update() override;
	void PostUpdate() override;
	
private:

	void ChangeBehavior(std::unique_ptr<ITutorialBehavior> _newBehavior);

	void ToGameScene();

private:

	// ------------------- camera ------------------- //
	std::unique_ptr<DebugCamera> debugCamera_;
	std::unique_ptr<FollowCamera> followCamera_;
	std::unique_ptr<Camera2d> camera2d_;

	// ------------------- collision ------------------- //
	std::unique_ptr<AOENGINE::CollisionManager> collisionManager_;
	std::unique_ptr<GameCallBacksManager> gameCallBacksManager_;

	// ------------------- sprite ------------------- //
	std::unique_ptr<CanvasUI> canvas_;
	std::unique_ptr<FadePanel> fadePanel_;

	std::unique_ptr<TutorialMissionGauge> tutorialMissionGauge_;

	// ------------------- actor ------------------- //
	Skybox* skybox_;
	std::unique_ptr<Floor> floor_;

	std::unique_ptr<PlayerManager> playerManager_;
	std::unique_ptr<EnemyManager> enemyManager_;

	std::unique_ptr<ITutorialBehavior> tutorialBehavior_;
};

