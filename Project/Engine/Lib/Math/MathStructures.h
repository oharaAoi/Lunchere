#pragma once
#include <string>
#include <vector>
#include "Engine/Lib/Math/Quaternion.h"
#include "Engine/Lib/Math/Vector3.h"
#include "Engine/Lib/Math/Vector4.h"
#include "Engine/Lib/Math/Matrix4x4.h"

namespace AOENGINE {

namespace Math {

/// <summary>
/// 拡縮、回転、移動
/// </summary>
struct SRT {
	Math::Vector3 scale = CVector3::UNIT;
	Math::Vector3 rotate = CVector3::ZERO;
	Math::Vector3 translate = CVector3::ZERO;

	Math::Matrix4x4 MakeAffine();
};

/// <summary>
/// 拡縮、回転(Math::Quaternion)、移動
/// </summary>
struct QuaternionSRT {
	Math::Vector3 scale = CVector3::UNIT;
	Math::Quaternion rotate = Math::Quaternion(0, 0, 0, 1);
	Math::Vector3 translate = CVector3::ZERO;

	Math::Matrix4x4 worldMat_;
	const Math::Matrix4x4* parentWorldMat = nullptr;

	// 関数
	Math::Matrix4x4 MakeAffine();

	void SetParent(const Math::Matrix4x4& parentMat);

	void LookAt(const Math::Vector3& target, const Math::Vector3& up = CVector3::UP);
};

/// <summary>
/// 球
/// </summary>
struct Sphere {
	Math::Vector3 center = CVector3::ZERO;
	float radius;
};

/// <summary>
/// 軸平行境界箱
/// </summary>
struct AABB {
	Math::Vector3 min;
	Math::Vector3 max;
	Math::Vector3 center; // 中心点
};

/// <summary>
/// 直線
/// </summary>
struct Line {
	Math::Vector3 origin; // 原点
	Math::Vector3 diff;	// 差分
};

/// <summary>
/// OBB(有向境界箱)
/// </summary>
struct OBB {
	Math::Vector3 center; // 中心点
	Math::Vector3 orientations[3]; // 座標軸、正規化、直交必須
	Math::Vector3 size; // 座標軸方向の長さの半分

	// 回転軸
	Math::Matrix4x4 matRotate;

	void MakeOBBAxis(const Math::Quaternion& rotate);
	std::vector<Math::Vector3> MakeIndex() const;
};

}

}

#ifndef AOENGINE_MATH_NAMESPACE_ALIAS
#define AOENGINE_MATH_NAMESPACE_ALIAS
namespace Math = AOENGINE::Math;
#endif
