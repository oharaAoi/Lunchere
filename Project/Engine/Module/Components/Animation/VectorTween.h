#pragma once
#include <utility>
#include "Engine/Lib/Color.h"
#include "Engine/Lib/Math/Vector2.h"
#include "Engine/Lib/Math/Vector3.h"
#include "Engine/Lib/Math/Vector4.h"
#include "Engine/Lib/Math/MyMath.h"
#include "Engine/Lib/Math/Easing.h"

enum class LoopType {
	Loop,
	Stop,
	Return,
	RoundTrip
};

namespace AOENGINE {

/// <summary>
/// Vector(float, Math::Vector2, Math::Vector3, Math::Vector4)関係をTween(Animation)させるクラス
/// </summary>
/// <typeparam name="T">: float, Math::Vector2, Math::Vector3, Math::Vector4</typeparam>
template <typename T>
/// <summary>
/// ベクトル補間クラス
/// </summary>
class VectorTween {
public:

	VectorTween() {};
	~VectorTween() {};

	template <typename T>
	void Init(const T& _start, const T& _end, float _duration, int _easeIndex, LoopType _loop) {
		value_ = T{};
		start_ = _start;
		end_ = _end;
		duration_ = _duration;
		easeIndex_ = _easeIndex;
		loopType_ = _loop;
		isFinish_ = false;
		isStop_ = false;
		isReturn_ = true;
		currentTime_ = 0.0f;
	}

	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="deltaTime">: デルタタイム</param>
	void Update(float deltaTime) {
		if (isStop_) { return; }
		if (currentTime_ >= duration_) {
			ChangeState();
			return;
		}

		currentTime_ += deltaTime;
		float t = currentTime_ / duration_;
		value_ = Lerp(start_, end_, Math::CallEasing(easeIndex_, t));
	};

	/// <summary>
	/// 現在の進行状態を変更するかを判別する関数
	/// </summary>
	void ChangeState() {
		switch (loopType_) {
		case LoopType::Loop:
			currentTime_ = 0.0f;
			break;
		case LoopType::Stop:
			isFinish_ = true;
			break;
		case LoopType::Return:
			std::swap(start_, end_);
			currentTime_ = 0.0f;
			break;
		case LoopType::RoundTrip:
			if (!isReturn_) { break; }
			std::swap(start_, end_);
			currentTime_ = 0.0f;
			isReturn_ = false;
			break;
		}
	}

public:

	/// <summary>
	/// 一時停止のフラグを設定
	/// </summary>
	/// <param name="flag"></param>
	void SetIsStop(bool flag) { isStop_ = flag; }

	bool GetIsFinish() const { return isFinish_; }

	void SetLoopType(LoopType type) { loopType_ = type; }

	void SetStart(const T& _start) { start_ = _start; }
	void SetEnd(const T& _end) { end_ = _end; }

	/// <summary>
	/// Tweenをやり直す
	/// </summary>
	void Reset() {
		currentTime_ = 0.0f;
		isFinish_ = false;
	}

	void SetCurrentTime(float time) { currentTime_ = time; }

	T GetValue() { return value_; }

private:

	T value_;			// 変更する値

	T start_;			// 進行開始地点
	T end_;				// 進行終了地点

	float currentTime_;	// 現在の進行度
	float duration_;	// 到達するまでの時間

	int easeIndex_;		// どのEasingを行うか

	LoopType loopType_;	// 1周目が終わった後にどのような動きをするか

	bool isStop_;		// tweenを一旦止めるかどうか

	bool isFinish_;		// 終了したかどうか

	bool isReturn_;		// リターンを行ったかどうか
};

}
