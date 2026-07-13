#pragma once
#include <cmath>
#include <algorithm>
#include "Engine/Lib/Math/Matrix3x3.h"

namespace AOENGINE {

namespace Math {

/// <summary>
/// 2次元ベクトル
/// </summary>
class Vector2 final {
public:
	float x;
	float y;

	constexpr Vector2() : x(0), y(0) {}
	constexpr Vector2(float x, float y) : x(x), y(y) {}

	// 加算
	Math::Vector2 operator+(const Math::Vector2& obj) const { return Math::Vector2(x + obj.x, y + obj.y); }
	Math::Vector2 operator+(float obj) const { return Math::Vector2(x + obj, y + obj); }

	// 加算代入
	Math::Vector2& operator+=(const Math::Vector2& obj) {
		x += obj.x;
		y += obj.y;
		return *this;
	}
	Math::Vector2& operator+=(float obj) {
		x += obj;
		y += obj;
		return *this;
	}

	// 減算
	Math::Vector2 operator-(const Math::Vector2& obj) const { return Math::Vector2(x - obj.x, y - obj.y); }
	Math::Vector2 operator-(float obj) const { return Math::Vector2(x - obj, y - obj); }

	// 減算代入
	Math::Vector2& operator-=(const Math::Vector2& obj) {
		x -= obj.x;
		y -= obj.y;
		return *this;
	}
	Math::Vector2& operator-=(float obj) {
		x -= obj;
		y -= obj;
		return *this;
	}

	// 乗算
	Math::Vector2 operator*(const Math::Vector2& obj) const { return Math::Vector2(x * obj.x, y * obj.y); }
	Math::Vector2 operator*(float obj) const { return Math::Vector2(x * obj, y * obj); }

	// 乗算代入
	Math::Vector2& operator*=(const Math::Vector2& obj) {
		x *= obj.x;
		y *= obj.y;
		return *this;
	}
	Math::Vector2& operator*=(float obj) {
		x *= obj;
		y *= obj;
		return *this;
	}

	// 除算
	Math::Vector2 operator/(const Math::Vector2& obj) const { return Math::Vector2(x / obj.x, y / obj.y); }
	Math::Vector2 operator/(float obj) const { return Math::Vector2(x / obj, y / obj); }

	// 乗算代入
	Math::Vector2& operator/=(const Math::Vector2& obj) {
		x /= obj.x;
		y /= obj.y;
		return *this;
	}
	Math::Vector2& operator/=(float obj) {
		x /= obj;
		y /= obj;
		return *this;
	}

	// =============================================
	// 数学用関数
	// =============================================

	/// <summary>
	/// ベクトルの長さ
	/// </summary>
	/// <returns></returns>
	float Length() const;

	/// <summary>
	/// 正規化
	/// </summary>
	/// <returns></returns>
	Math::Vector2 Normalize() const;

	/// <summary>
	/// 範囲内に収める
	/// </summary>
	/// <param name="min"></param>
	/// <param name="max"></param>
	/// <returns></returns>
	void Clamp(const Math::Vector2& min, const Math::Vector2& max);

	/// <summary>
	/// 内積
	/// </summary>
	/// <param name="v1">: ベクトル1</param>
	/// <param name="v2">: ベクトル2</param>
	/// <returns></returns>
	static float Dot(const Math::Vector2& v1, const Math::Vector2& v2);

	/// <summary>
	/// 外積
	/// </summary>
	/// <param name="v1">: ベクトル1</param>
	/// <param name="v2">: ベクトル2</param>
	/// <returns></returns>
	static float Cross(const Math::Vector2& v1, const Math::Vector2& v2);

	/// <summary>
	/// 線形補完
	/// </summary>
	/// <param name="st">: 開始位置</param>
	/// <param name="end">: 終了位置</param>
	/// <param name="t"></param>
	/// <returns></returns>
	static Math::Vector2 Lerp(const Math::Vector2& st, const Math::Vector2& end, float t);

	/*拡縮行列*/
	static Math::Matrix3x3 MakeScaleMat(const Math::Vector2& scale);

	/*回転行列*/
	static Math::Matrix3x3 MakeRotateMat(float theta);

	/*平行移動行列*/
	static Math::Matrix3x3 MakeTranslateMat(const Math::Vector2& translate);

	/*アフィン*/
	static Math::Matrix3x3 MakeAffine(const Math::Vector2& scale, float theta, const Math::Vector2& translate);
};

}

}

#ifndef AOENGINE_MATH_NAMESPACE_ALIAS
#define AOENGINE_MATH_NAMESPACE_ALIAS
namespace Math = AOENGINE::Math;
#endif


/// <summary>
/// Math::Vector2定数
/// </summary>
namespace CMath::Vector2 {
constexpr Math::Vector2 RIGHT{ 1.0f, 0.0f };
constexpr Math::Vector2 UP{ 0.0f, 1.0f };
constexpr Math::Vector2 ZERO{ 0.0f, 0.0f };
constexpr Math::Vector2 UNIT{ 1.0f, 1.0f };
}