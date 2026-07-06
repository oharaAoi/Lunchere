#include "TutorialScene.h"
#include "Engine/Core/Engine.h"
#include "Engine/Render/Render.h"
#include "Engine/Lib/Json/JsonItems.h"
#include "Engine/System/Manager/CollisionLayerManager.h"
#include "Engine/System/Input/Input.h"
#include "Game/Information/ColliderCategory.h"
#include "Game/Scene/SceneBehavior/TutorialBehavior.h"

TutorialScene::TutorialScene() {}
TutorialScene::~TutorialScene() { Finalize(); }

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　終了処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void TutorialScene::Finalize() {}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　初期化
//////////////////////////////////////////////////////////////////////////////////////////////////

void TutorialScene::Init() {
	SetSceneName("TutorialScene");
	Engine::GetCanvas2d()->Init();

	AOENGINE::JsonItems* adjust = AOENGINE::JsonItems::GetInstance();
	adjust->Init();

	auto& layers = AOENGINE::CollisionLayerManager::GetInstance();
	layers.RegisterCategoryList(GetColliderTagsList());

	// -------------------------------------------------
	// ↓ Sceneの初期化
	// -------------------------------------------------
	collisionManager_ = std::make_unique<AOENGINE::CollisionManager>();
	collisionManager_->Init();

	AOENGINE::Render::GetLightGroup()->Load();
	LoadSceneEffect();
	LoadScene("./Project/Packages/Game/Scene/", "tutorial", ".json");

	// -------------------------------------------------
	// ↓ cameraの初期化
	// -------------------------------------------------

	followCamera_ = std::make_unique<FollowCamera>();
	debugCamera_ = std::make_unique<DebugCamera>();
	camera2d_ = std::make_unique<Camera2d>();
	followCamera_->Init();
	debugCamera_->Init();
	camera2d_->Init();

	// -------------------------------------------------
	// ↓ actorの初期化
	// -------------------------------------------------

	floor_ = std::make_unique<Floor>();
	floor_->Init();

	playerManager_ = std::make_unique<PlayerManager>();
	playerManager_->Init();

	enemyManager_ = std::make_unique<EnemyManager>();
	enemyManager_->Init();

	// -------------------------------------------------
	// ↓ managerの初期化
	// -------------------------------------------------

	gameCallBacksManager_ = std::make_unique<GameCallBacksManager>();
	gameCallBacksManager_->SetBossRoot(enemyManager_->GetBossRoot());
	gameCallBacksManager_->SetPlayerManager(playerManager_.get());
	gameCallBacksManager_->SetGround(floor_.get());
	gameCallBacksManager_->Init(collisionManager_.get());

	// -------------------------------------------------
	// ↓ spriteの初期化
	// -------------------------------------------------
	canvas_ = std::make_unique<CanvasUI>();
	canvas_->SetPlayer(playerManager_->GetPlayer());
	canvas_->SetBoss(enemyManager_->GetBoss());
	canvas_->SetFollowCamera(followCamera_.get());
	canvas_->SetEnemyManager(enemyManager_.get());
	canvas_->Init(true);

	tutorialMissionGauge_ = std::make_unique<TutorialMissionGauge>();
	tutorialMissionGauge_->Init();

	fadePanel_ = std::make_unique<FadePanel>();
	fadePanel_->Init();
	fadePanel_->SetColor(AOENGINE::Color(0, 0, 0, 0));

	// -------------------------------------------------
	// ↓ その他設定
	// -------------------------------------------------
	ChangeBehavior(std::make_unique<TutorialMoveBehavior>(this));

	Player* pPlayer = playerManager_->GetPlayer();
	pPlayer->SetFollowCamera(followCamera_.get());
	pPlayer->SetReticle(canvas_->GetReticle());

	enemyManager_->SetPlayer(pPlayer);

	BossRoot* bossRoot = enemyManager_->GetBossRoot();
	bossRoot->SetUIs(canvas_->GetBossUIs());
	bossRoot->SetPlayer(playerManager_->GetPlayer());
	bossRoot->SetTargetTransform(playerManager_->GetPlayer()->GetTransform());
	bossRoot->SetFollowCamera(followCamera_.get());

	followCamera_->SetTarget(playerManager_->GetPlayer());
	followCamera_->SetReticle(canvas_->GetReticle());

	AOENGINE::DirectionalLight* light = AOENGINE::Render::GetLightGroup()->GetDirectionalLight();
	light->SetIntensity(0.5f);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　更新
//////////////////////////////////////////////////////////////////////////////////////////////////

void TutorialScene::Update() {
	// -------------------------------------------------
	// ↓ actorの更新
	// -------------------------------------------------

	playerManager_->Update();
	enemyManager_->GetBoss()->GetBehaviorTree()->SetExecute(false);
	enemyManager_->Update();

	tutorialMissionGauge_->Update();
	tutorialBehavior_->Update();

	// -------------------------------------------------
	// ↓ spriteの更新
	// -------------------------------------------------
	canvas_->Update();
	fadePanel_->Update();

	if (fadePanel_->GetIsFinished()) {
		nextSceneType_ = SceneType::Game;
	}

	AOENGINE::Input* input = AOENGINE::Input::GetInstance();
	if (input->IsTriggerButton(XInputButtons::Start)) {
		this->ToGameScene();
	}

	// -------------------------------------------------
	// ↓ cameraの更新 
	// -------------------------------------------------
	if (debugCamera_->GetIsActive()) {
		debugCamera_->Update();
	} else {
		followCamera_->Update();
	}
	camera2d_->Update();
}

void TutorialScene::PostUpdate() {
	gameCallBacksManager_->Update();
	playerManager_->PostUpdate();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　behaviorの更新
//////////////////////////////////////////////////////////////////////////////////////////////////

void TutorialScene::ChangeBehavior(std::unique_ptr<ITutorialBehavior> _newBehavior) {
	tutorialBehavior_ = std::move(_newBehavior);
	if (tutorialBehavior_) {
		tutorialBehavior_->Init();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　gameSceneへのfade
//////////////////////////////////////////////////////////////////////////////////////////////////

void TutorialScene::ToGameScene() {
	fadePanel_->SetBlackOut(3.0f);
}
