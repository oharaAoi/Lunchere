#include "TitleScene.h"
#include "Engine/Core/Engine.h"
#include "Engine/Render/Render.h"
#include "Engine/System/Input/Input.h"
#include "Engine/Module/Components/Light/DirectionalLight.h"
#include "Engine/Lib/Json/JsonItems.h"

TitleScene::TitleScene() {}
TitleScene::~TitleScene() {Finalize();}

void TitleScene::Finalize() {}

void TitleScene::Init() {
	SetSceneName("TitleScene");
	Engine::GetCanvas2d()->Init();

	AOENGINE::JsonItems* adjust = AOENGINE::JsonItems::GetInstance();
	adjust->Init();

	LoadSceneEffect();
	LoadScene("./Project/Packages/Game/Scene/", "template", ".json");

	// -------------------------------------------------
	// ↓ cameraの初期化
	// -------------------------------------------------
	camera2d_ = std::make_unique<Camera2d>();
	camera3d_ = std::make_unique<Camera3d>();
	debugCamera_ = std::make_unique<DebugCamera>();

	camera2d_->Init();
	camera3d_->Init();
	debugCamera_->Init();

	fadePanel_ = std::make_unique<FadePanel>();
	fadePanel_->Init();
	fadePanel_->SetBlackOutOpen();

	titleUIs_ = std::make_unique<TitleUIs>();
	titleUIs_->Init();

	ChangeBehavior(new TitlePushSpaceBehavior(this));

	AOENGINE::DirectionalLight* light = AOENGINE::Render::GetLightGroup()->GetDirectionalLight();
	light->SetIntensity(0.3f);

	putButton_ = false;
}

void TitleScene::Update() {
	fadePanel_->Update();

	titleBehavior_->Update();

	titleUIs_->Update();

	// -------------------------------------------------
	// ↓ cameraの更新 
	// -------------------------------------------------
	if (debugCamera_->GetIsActive()) {
		debugCamera_->Update();
	} else {
		camera3d_->Update();
	}
	camera2d_->Update();
}

void TitleScene::ChangeBehavior(ITitleBahavior* _newBehavior) {
	titleBehavior_.reset(_newBehavior);
	if (titleBehavior_) {
		titleBehavior_->Init();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ pushSpace用のBehavior
///////////////////////////////////////////////////////////////////////////////////////////////

void TitlePushSpaceBehavior::Init() {
	putButton_ = false;
}

void TitlePushSpaceBehavior::Update() {
	if (!putButton_) {
		if (AOENGINE::Input::GetInstance()->IsTriggerButton(XInputButtons::ButtonA)) {
			putButton_ = true;
			host_->GetFadePanel()->SetBlackOut();
		}
	} else {
		if (host_->GetFadePanel()->GetIsFinished()) {
			host_->ChangeBehavior(new TitleGameModeGuideBehavior(host_));
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ GameMode選択用のBehavior
///////////////////////////////////////////////////////////////////////////////////////////////

void TitleGameModeGuideBehavior::Init() {
	gameModeGuide_ = std::make_unique<GameModeGuide>();
	gameModeGuide_->Init();
	gameModeGuide_->Open();
	host_->GetFadePanel()->SetColor(Colors::Linear::black);
}

void TitleGameModeGuideBehavior::Update() {
	gameModeGuide_->Update();

	if (gameModeGuide_->Decide()) {
		if (gameModeGuide_->GetSelectModeType() == SelectModeType::ToGame) {
			host_->SetNextSceneType(SceneType::Game);
		} else if (gameModeGuide_->GetSelectModeType() == SelectModeType::ToTutorial) {
			host_->SetNextSceneType(SceneType::Tutorial);
		}
	}
}
