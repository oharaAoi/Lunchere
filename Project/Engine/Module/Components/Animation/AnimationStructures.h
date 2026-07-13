#pragma once
#include <map>
#include <vector>
#include <string>
#include "Engine/Lib/Math/Vector3.h"
#include "Engine/Lib/Math/Quaternion.h"

namespace AOENGINE {

/// <summary>
/// キーフレーム
/// </summary>
/// <typeparam name="tValue"></typeparam>
template <typename tValue>
/// <summary>
/// アニメーションキーフレーム情報
/// </summary>
struct Keyframe {
	float time;
	tValue value;
};

using KeyframeVector3 = Keyframe<Math::Vector3>;
using KeyframeQuaternion = Keyframe<Math::Quaternion>;

/// <summary>
/// Animationのカーブ
/// </summary>
/// <typeparam name="tValue"></typeparam>
template <typename tValue>
/// <summary>
/// アニメーションカーブ情報
/// </summary>
struct AnimationCurve {
	std::vector<Keyframe<tValue>> keyframes;
};

/// <summary>
/// Nodeごとのアニメーション
/// </summary>
struct NodeAnimation {
	AnimationCurve<Math::Vector3> translate;
	AnimationCurve<Math::Quaternion> rotate;
	AnimationCurve<Math::Vector3> scale;
};

/// <summary>
/// アニメーションの情報
/// </summary>
struct Animation {
	float duration;		// アニメーション全体の尺
	// NodeAnimationの集合。Node名で引けるようにしておく
	std::map<std::string, NodeAnimation> nodeAnimations;
	std::string animationName;
};

}

using AOENGINE::Keyframe;
using AOENGINE::KeyframeVector3;
using AOENGINE::KeyframeQuaternion;
using AOENGINE::AnimationCurve;
using AOENGINE::NodeAnimation;
using AOENGINE::Animation;
