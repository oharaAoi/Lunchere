#pragma once
#define DIRECTINPUT_VERSION 0x0800
#define NOMINMAX
#include <dinput.h>
#include <wrl.h>
#include <memory>
#include <cassert>
#include <Xinput.h>
#include <limits>
#include "Engine/Lib/Math/Vector2.h"
#include "Engine/Lib/Math/MyMath.h"

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "Xinput.lib")

enum XInputButtons {
	DpadUp = 0x0001,
	DpadDown = 0x0002,
	DpadLeft = 0x0004,
	DpadRight = 0x0008,
	Start = 0x0010,
	Back = 0x0020,
	LStickThumb = 0x0040,
	RStickThumb = 0x0080,
	LShoulder = 0x0100,
	RShoulder = 0x0200,
	LtShoulder = 0x0400,
	RtShoulder = 0x0800,
	ButtonA = 0x1000,
	ButtonB = 0x2000,
	ButtonX = 0x4000,
	ButtonY = 0x8000
};

enum InputDevice {
	Keybord,
	Gamepad
};

namespace AOENGINE {

/// <summary>
/// コントローラー振動情報
/// </summary>
struct ControllerVibration {
	float leftStrength = 0.0f;
	float rightStrength = 0.0f;
	float duration = 0.0f;
	float timer = 0.0f;
	bool active = false;
};

// デッドゾーン
const float DEADZONE = 0.1f;

/// <summary>
/// 入力処理
/// </summary>
class Input {
public:

	Input();
	~Input();
	Input(const Input&) = delete;
	const Input& operator=(const Input&) = delete;

	void Finalize();

	/// <summary>
	/// 初期化関数
	/// </summary>
	void Init(WNDCLASS wCalss, HWND hwnd);

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

public:

	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	/// <returns></returns>
	static  AOENGINE::Input* GetInstance();

	static BYTE GetKey(uint8_t keyNum) { return key_[keyNum]; }

public:
	/// <summary>
	/// キーボードの初期化
	/// </summary>
	/// <param name="wCalss"></param>
	/// <param name="hwnd"></param>
	void KeyboardInitialize(HWND hwnd);

	/// <summary>
	/// マウスの初期化
	/// </summary>
	/// <param name="wCalss"></param>
	/// <param name="hwnd"></param>
	void MouseInitialize();

	/// <summary>
	/// ゲームパッドの初期化
	/// </summary>
	void GamePadInitialize();

public: // 入力

	// 入力を受け付けない用にする
	static bool GetNotAccepted() { return notAccepted_; }
	static void SetNotAccepted(bool notAccepted) { notAccepted_ = notAccepted; }

	// ---------------------------------------------------------------
	// ↓　キーボード
	// ---------------------------------------------------------------
	// キーを押した瞬間か
	static bool IsTriggerKey(uint8_t keyNum);
	// キーを離した瞬間か
	static bool IsReleaseKey(uint8_t keyNum);
	// キーを押した状態か
	static bool IsPressKey(uint8_t keyNum);
	// キーを離した状態か
	static bool IsUnPressKey(uint8_t keyNum);

	// ---------------------------------------------------------------
	// ↓　マウス
	// ---------------------------------------------------------------
	// キーを押した瞬間か
	static bool IsTriggerMouse(uint8_t keyNum);
	// キーを離した瞬間か
	static bool IsReleaseMouse(uint8_t keyNum);
	// キーを押した状態か
	static bool IsPressMouse(uint8_t keyNum);
	// キーを離した状態か
	static bool IsUnPressMouse(uint8_t keyNum);

	static int GetWheel();

	/// <summary>
	/// マウスの座標を返す
	/// </summary>
	/// <returns></returns>
	static Math::Vector2 GetMousePosition() { return { static_cast<float>(mousePoint_.x),static_cast<float>(mousePoint_.y) }; }

	/// <summary>
	/// マウスの移動量を返す
	/// </summary>
	/// <returns></returns>
	static Math::Vector2 GetMouseVelocity() {
		return Math::Vector2(static_cast<float>(currentMouse_.lX), static_cast<float>(currentMouse_.lY));
	}

	// ---------------------------------------------------------------
	// ↓　ゲームパッド
	// ---------------------------------------------------------------
	// ゲームパッドのボタンを取得
	static bool IsTriggerButton(const XInputButtons& bottons);
	// ゲームパッドのボタンを取得(長押し)
	static bool IsPressButton(const XInputButtons& bottons);
	// 左ジョイスティックの値の取得
	static Math::Vector2 GetLeftJoyStick(float deadZone = 0.1f);
	// 右ジョイスティックの値の取得
	static Math::Vector2 GetRightJoyStick(float deadZone = 0.1f);
	// コントローラーと繋がっているか
	static bool IsControllerConnected();

	// ---------------------------------------------------------------
	// ↓　振動
	// ---------------------------------------------------------------

	static void Vibrate(float strength, float duration);
	static void ApplyVibrate();

private:

	bool IsThumbLT();
	bool IsThumbRT();

private:
	// DirectInputオブジェクトの生成
	Microsoft::WRL::ComPtr<IDirectInput8> directInput_ = nullptr;
	// keyboardデバイスの生成
	Microsoft::WRL::ComPtr<IDirectInputDevice8> keyboard_ = nullptr;
	// mouseデバイスの生成
	Microsoft::WRL::ComPtr<IDirectInputDevice8> mouse_ = nullptr;
	// padデバイスの生成
	Microsoft::WRL::ComPtr<IDirectInputDevice8> gamepad = nullptr;

	// keyboard
	static BYTE key_[256];
	static BYTE preKey_[256];

	// mouse 
	static DIMOUSESTATE currentMouse_;
	static DIMOUSESTATE preMouse_;
	static POINT mousePoint_;

	// pad
	static XINPUT_STATE gamepadState_;
	static XINPUT_STATE preGamepadState_;

	static InputDevice inputDevice_;

	// 入力を受け付けない用にする
	static bool notAccepted_;

	static ControllerVibration vibrations_;
};
}