#include "FadePanel.h"
#include "Engine/Core/Engine.h"
#include "Engine/WinApp/WinApp.h"
#include "Engine/Lib/GameTimer.h"

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void FadePanel::Init() {
	time_ = 0.0f;
	timeLimit_ = 1.0f;
	panelAlpha_ = 0.0f;

	doNothing_ = true;
	isBlackOut_ = false;
	isFinished_ = false;

	panel_ = Engine::GetCanvas2d()->AddSprite("panel.png", "panel", 100);
	panel_->Load("UI", "panel");
	panel_->SetIsActive(true);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void FadePanel::Update() {
	if (doNothing_) {
		return;
	}

	if (time_ >= timeLimit_) {
		isFinished_ = true;
		doNothing_ = true;
		return;
	}

	// -------------------------------------------------
	// ↓ 実際にカラーを調整する
	// -------------------------------------------------
	if (isBlackOut_) {	// ブラックアウトさせる
		time_ += AOENGINE::GameTimer::DeltaTime();
		time_ = std::clamp(time_, 0.0f, timeLimit_);

		float t = time_ / timeLimit_;
		panelAlpha_ = std::lerp(0.0f, 1.0f, t);

		panel_->SetColor(AOENGINE::Color(0.0f, 0.0f, 0.0f, panelAlpha_));

	} else {			// ブラックアウトから抜ける
		time_ += AOENGINE::GameTimer::DeltaTime();
		time_ = std::clamp(time_, 0.0f, timeLimit_);

		float t = time_ / timeLimit_;
		panelAlpha_ = std::lerp(1.0f, 0.0f, t);

		panel_->SetColor(AOENGINE::Color(0.0f, 0.0f, 0.0f, panelAlpha_));
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集処理
///////////////////////////////////////////////////////////////////////////////////////////////

void FadePanel::Debug_Gui() {
	static float timeLimit = 1;
	ImGui::DragFloat("timeLimit", &timeLimit);

	if (ImGui::Button("SetBlackOut")) {
		SetBlackOut(timeLimit);
	}

	if (ImGui::Button("SetBlackOutOpen")) {
		SetBlackOutOpen(timeLimit);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ blackOutさせる処理
///////////////////////////////////////////////////////////////////////////////////////////////

void FadePanel::SetBlackOut(float timeLimit) {
	if (!doNothing_) {
		return;
	}

	isBlackOut_ = true;
	doNothing_ = false;
	isFinished_ = false;
	time_ = 0.0f;
	timeLimit_ = timeLimit;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ blackOutから抜ける処理
///////////////////////////////////////////////////////////////////////////////////////////////

void FadePanel::SetBlackOutOpen(float timeLimit) {
	if (!doNothing_) {
		return;
	}

	isBlackOut_ = false;
	doNothing_ = false;
	isFinished_ = false;
	time_ = 0.0f;
	timeLimit_ = timeLimit;
}

void FadePanel::SetColor(const AOENGINE::Color& _color) {
	panel_->SetColor(_color);
}