#pragma once
#include <vector>
#include "Engine/Lib/Math/MathStructures.h"

namespace AOENGINE {

template<typename RotateT>
/// <summary>
/// トランスフォームキーフレーム情報
/// </summary>
struct TransformKeyFrame {
	float time = 0.0f;
	Math::Vector3 scale{};
	RotateT rotate{};
	Math::Vector3 translate{};
};

template<typename SRTType>
struct KeyFrameTraits;

template<>
/// <summary>
/// キーフレーム型特性
/// </summary>
struct KeyFrameTraits<Math::SRT> {
	using RotateType = Math::Vector3;
	using KeyFrameType = TransformKeyFrame<RotateType>;
};

template<>
/// <summary>
/// キーフレーム型特性
/// </summary>
struct KeyFrameTraits<Math::QuaternionSRT> {
	using RotateType = Math::Quaternion;
	using KeyFrameType = TransformKeyFrame<RotateType>;
};

/// <summary>
/// Transform関連のアニメーション管理するクラス
/// </summary>
/// <typeparam name="T">WorldTransform or ScreenTransform</typeparam>
template<typename T>
/// <summary>
/// トランスフォームアニメータークラス
/// </summary>
class TransformAnimator {
public:

	using SRTType = typename T::SRTType;
	using KeyFrameType = typename KeyFrameTraits<SRTType>::KeyFrameType;

public:

	TransformAnimator() = default;
	~TransformAnimator() = default;

public:

	/// <summary>
	/// 実行を行う
	/// </summary>
	/// <returns></returns>
	bool Run();

	/// <summary>
	/// 編集を行う
	/// </summary>
	void Debug_Gui();

	/// <summary>
	/// Transformを設定する
	/// </summary>
	/// <param name="t"></param>
	void SetTransform(T* t) {
		pTransform_ = t;
	}

	void AddKeyFrame(float time) {
		if (!pTransform_) {
			return;
		}

		const auto& srt = pTransform_->GetSRT();

		KeyFrameType key{};
		key.time = time;
		key.scale = srt.scale;
		key.rotate = srt.rotate;
		key.translate = srt.translate;

		keyFrames_.push_back(key);
	}

private:

	T* pTransform_ = nullptr;

	std::vector<KeyFrameType> keyFrames_;

};

template<typename T>
inline bool TransformAnimator<T>::Run() {
	return false;
}

}