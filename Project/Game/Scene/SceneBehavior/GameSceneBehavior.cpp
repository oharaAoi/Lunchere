#include "GameSceneBehavior.h"
#include "Engine/System/Input/Input.h"
#include "Engine/Lib/GameTimer.h"

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ GamePlay中
///////////////////////////////////////////////////////////////////////////////////////////////

void GamePlayBehavior::Init() {
}

void GamePlayBehavior::Update() {
	host_->CharacterUpdate();

	AOENGINE::Input* input = AOENGINE::Input::GetInstance();
	if (input->IsTriggerButton(XInputButtons::Start)) {
		host_->ChangeBehavior(std::make_unique<GameMenuBehavior>(host_));

		AOENGINE::GameTimer::SetTimeScale(0.0f);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ GameMenu
///////////////////////////////////////////////////////////////////////////////////////////////

void GameMenuBehavior::Init() {
	gameMenuUI_ = std::make_unique<GameMenuUI>();
	gameMenuUI_->Init();
}

void GameMenuBehavior::Update() {
	gameMenuUI_->Update();

	GameMenuSelectItem item = gameMenuUI_->Decide();
	if (item == GameMenuSelectItem::Go_Title) {
		host_->SetNextSceneType(SceneType::Title);
		AOENGINE::GameTimer::SetTimeScale(1.0f);
	} else if(item == GameMenuSelectItem::Go_Game) {
		host_->ChangeBehavior(std::make_unique<GamePlayBehavior>(host_));
		AOENGINE::GameTimer::SetTimeScale(1.0f);
	}
}
