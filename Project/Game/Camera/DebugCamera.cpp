#include "DebugCamera.h"
#include "Engine/System/Editor/Window/EditorWindows.h"
#include "Engine/System/Input/Input.h"
#include "Engine/Render/Render.h"

namespace {
constexpr float kDefaultMoveBaseSpeed = 15.0f;
constexpr float kSprintSpeedMultiplier = 2.0f;
constexpr Math::Vector3 kResetCameraPosition{ 0.0f, 5.0f, -30.0f };
}

DebugCamera::DebugCamera() {}
DebugCamera::~DebugCamera() {}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　初期化
//////////////////////////////////////////////////////////////////////////////////////////////////

void DebugCamera::Finalize() {
}

void DebugCamera::Init() {
	BaseCamera::Init();
	
	moveBaseSpeed_ = kDefaultMoveBaseSpeed;
	moveSpeed_ = moveBaseSpeed_;
	moveRotate_ = transform_.rotate;
	preMoveRotate_ = transform_.rotate;

	isFocused_ = true;

	isActive_ = false;

	AOENGINE::EditorWindows::AddObjectWindow(this, "debugCamera");
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　更新処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void DebugCamera::Update() {
	if (isFocused_) {
		RotateMove();
		TransitionMove();
	}

	moveRotate_ = moveRotate_.Normalize();
	transform_.rotate = moveRotate_;

	BaseCamera::Update();

	AOENGINE::Render::SetEyePos(GetWorldPosition());
	AOENGINE::Render::SetViewProjection(viewMatrix_, projectionMatrix_);
	AOENGINE::Render::SetCameraRotate(transform_.rotate);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　編集処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void DebugCamera::Debug_Gui() {
	Math::Vector3 right = moveRotate_.MakeRight();
	Math::Vector3 up = moveRotate_.MakeUp();
	Math::Vector3 forward = moveRotate_.MakeForward();

	ImGui::DragFloat3("translate", &transform_.translate.x, 0.1f);
	ImGui::DragFloat4("rotate", &moveRotate_.x, 0.01f);
	ImGui::DragFloat("sensitivity", &sensitivity_, 0.01f, 0.0f, 0.1f);
	ImGui::Separator();
	ImGui::DragFloat4("qYaw", &qYaw.x, 0.01f);
	ImGui::DragFloat4("qPitch", &qPitch.x, 0.01f);

	if (ImGui::Button("Reset")) {
		transform_ = { CVector3::UNIT, Math::Quaternion(), kResetCameraPosition };
		moveRotate_ = Math::Quaternion();
		yaw_ = 0.0f;
		pitch_ = 0.0f;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　xy軸方向移動
//////////////////////////////////////////////////////////////////////////////////////////////////

void DebugCamera::TransitionMove() {

	moveDirection_ = Math::Vector3();

	if (AOENGINE::Input::IsPressKey(DIK_A)) {
		moveDirection_ -= moveRotate_.MakeRight() * moveSpeed_;
	}

	if (AOENGINE::Input::IsPressKey(DIK_D)) {
		moveDirection_ += moveRotate_.MakeRight() * moveSpeed_;
	}

	if (AOENGINE::Input::IsPressKey(DIK_W)) {
		moveDirection_ += moveRotate_.MakeForward() * moveSpeed_;
	}

	if (AOENGINE::Input::IsPressKey(DIK_S)) {
		moveDirection_ -= moveRotate_.MakeForward() * moveSpeed_;
	}

	if (AOENGINE::Input::IsPressKey(DIK_Q)) {
		moveDirection_ += moveRotate_.MakeUp() * moveSpeed_;
	}

	if (AOENGINE::Input::IsPressKey(DIK_E)) {
		moveDirection_ -= moveRotate_.MakeUp() * moveSpeed_;
	}

	if (AOENGINE::Input::IsPressKey(DIK_LSHIFT)) {
		moveSpeed_ = moveBaseSpeed_ * kSprintSpeedMultiplier;
	} else {
		moveSpeed_ = moveBaseSpeed_;
	}
	
	transform_.translate += moveDirection_ * kCameraDeltaTime_;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　回転移動(FPS視点)
//////////////////////////////////////////////////////////////////////////////////////////////////

void DebugCamera::RotateMove() {
	if (AOENGINE::Input::IsPressMouse(1)) {

		Math::Vector2 dire = AOENGINE::Input::GetMousePosition() - preMousePos_;

		// Y軸回転(Y軸回転は必ずworld空間での回転が行われる)
		yaw_ += dire.x * sensitivity_ * kCameraDeltaTime_;
		qYaw = Math::Quaternion::AngleAxis(yaw_, Math::Vector3(0.0f, 1.0f, 0.0f)).Normalize();

		// X軸回転(X軸回転は必ずlocal空間で回転が行われる)
		pitch_ += dire.y * sensitivity_ * kCameraDeltaTime_;
		qPitch = Math::Quaternion::AngleAxis(pitch_, Math::Vector3(1.0f, 0.0f, 0.0f)).Normalize();

		// 回転合成
		moveRotate_ = (qYaw * preMoveRotate_ * qPitch).Normalize();

	} else {
		preMoveRotate_ = moveRotate_;
		yaw_ = 0;
		pitch_ = 0;
	}

	preMousePos_ = AOENGINE::Input::GetMousePosition();
}
