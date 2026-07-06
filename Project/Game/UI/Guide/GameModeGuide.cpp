#include "GameModeGuide.h"
#include "Engine/Core/Engine.h"
#include "Engine/Lib/GameTimer.h"
#include "Engine/System/Editor/Window/EditorWindows.h"
#include "Engine/System/Input/Input.h"

namespace {
constexpr int kGuideRenderQueue = 200;
constexpr int kSelectRenderQueue = 1000;
constexpr float kSelectCoolTime = 0.2f;
constexpr float kSelectStickDeadZone = 0.5f;
}

GameModeGuide::GameModeGuide() = default;
GameModeGuide::~GameModeGuide() = default;

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void GameModeGuide::Init() {
	SetName("GameModeGuide");
	AOENGINE::Canvas2d* canvas = Engine::GetCanvas2d();
	goGame_ = canvas->AddSprite("goGame.png", "goGame", kGuideRenderQueue);
	goTutorial_ = canvas->AddSprite("goTutorial.png", "goTutorial", kGuideRenderQueue);
	select_ = canvas->AddSprite("gradation.png", "select", kSelectRenderQueue);

	goGame_->Load("Guide","goGame");
	goTutorial_->Load("Guide","goTutorial");
	
	goGame_->SetIsActive(false);
	goTutorial_->SetIsActive(false);
	select_->SetIsActive(false);

	AddChild(goGame_);
	AddChild(goTutorial_);
	AddChild(select_);

	selectModeType_ = SelectModeType::ToTutorial;
	selectModeIndex_ = static_cast<int>(SelectModeType::ToTutorial);
	isOpen_ = false;
	isSelectCoolTime_ = false;
	timer_ = 0.0f;

	AOENGINE::EditorWindows::AddObjectWindow(this, GetName());
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void GameModeGuide::Update() {
	if (!isOpen_) { return; }

	if (!isSelectCoolTime_) {
		isSelectCoolTime_ = Select();
	} else {
		timer_ += AOENGINE::GameTimer::DeltaTime();

		if (timer_ >= kSelectCoolTime) {
			isSelectCoolTime_ = false;
			timer_ = 0.0f;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集処理
///////////////////////////////////////////////////////////////////////////////////////////////

void GameModeGuide::Debug_Gui() {
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 開く
///////////////////////////////////////////////////////////////////////////////////////////////

void GameModeGuide::Open() {
	goGame_->SetIsActive(true);
	goTutorial_->SetIsActive(true);
	select_->SetIsActive(true);
	isOpen_ = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 決定処理
///////////////////////////////////////////////////////////////////////////////////////////////

bool GameModeGuide::Decide() {
	if (!isOpen_) { return false; }

	if (AOENGINE::Input::GetInstance()->IsTriggerButton(XInputButtons::ButtonA)) {
		return true;
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 選択
///////////////////////////////////////////////////////////////////////////////////////////////

bool GameModeGuide::Select() {
	// 選択する
	bool isSelect = false;
	if (AOENGINE::Input::GetInstance()->IsTriggerButton(XInputButtons::DpadUp) || AOENGINE::Input::GetInstance()->GetLeftJoyStick(kSelectStickDeadZone).y > 0.0f) {
		selectModeIndex_++;
		isSelect = true;
	}

	if (AOENGINE::Input::GetInstance()->IsTriggerButton(XInputButtons::DpadDown) || AOENGINE::Input::GetInstance()->GetLeftJoyStick(kSelectStickDeadZone).y < 0.0f) {
		selectModeIndex_--;
		isSelect = true;
	}

	// clamp
	if (selectModeIndex_ < 0) {
		selectModeIndex_ = static_cast<int>(SelectModeType::kMax) - 1;
	} else if (selectModeIndex_ >= static_cast<int>(SelectModeType::kMax)) {
		selectModeIndex_ = 0;
	}

	selectModeType_ = static_cast<SelectModeType>(selectModeIndex_);

	// UIを揃える
	if (selectModeType_ == SelectModeType::ToGame) {
		select_->SetTranslate(goGame_->GetTranslate());
	} else if (selectModeType_ == SelectModeType::ToTutorial) {
		select_->SetTranslate(goTutorial_->GetTranslate());
	}

	return isSelect;
}
