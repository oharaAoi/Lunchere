#pragma once
#include <cmath>
#include <algorithm>
#include "Matrix4x4.h"

namespace AOENGINE {

namespace Math {

/// <summary>
/// 3次元ベクトル
/// </summary>
class Vector3 final {
public:

	float x;
	float y;
	float z;

	constexpr Math::Vector3() : x(0), y(0), z(0) {}
	constexpr Math::Vector3(float x, float y, float z) : x(x), y(y), z(z) {}

	// =============================================
	/// <summary>
	/// 加算
	/// </summary>
	/// <param name="obj"></param>
	/// <returns></returns>
	Math::Vector3 operator+(const Math::Vector3& obj) const { return Math::Vector3(x + obj.x, y + obj.y, z + obj.z); }

	Math::Vector3 operator+(const float& obj) const { return Math::Vector3(x + obj, y + obj, z + obj); }

	Math::Vector3 operator+=(const Math::Vector3& obj) {
		x += obj.x;
		y += obj.y;
		z += obj.z;
		return *this;
	}

	Math::Vector3 operator+=(const float& obj) {
		x += obj;
		y += obj;
		z += obj;
		return *this;
	}

	/// <summary>
	/// 減算
	/// </summary>
	/// <param name="obj"></param>
	/// <returns></returns>
	Math::Vector3 operator-(const Math::Vector3& obj) const { return Math::Vector3(x - obj.x, y - obj.y, z - obj.z); }

	Math::Vector3 operator-(const float& obj) const { return Math::Vector3(x - obj, y - obj, z - obj); }

	Math::Vector3 operator-=(const Math::Vector3& obj) {
		x -= obj.x;
		y -= obj.y;
		z -= obj.z;
		return *this;
	}

	Math::Vector3 operator-=(const float& obj) {
		x -= obj;
		y -= obj;
		z -= obj;
		return *this;
	}

	// 単項マイナス
	Math::Vector3 operator-() const {
		return Math::Vector3{ -x, -y, -z };
	}

	/// <summary>
	/// 乗算
	/// </summary>
	/// <param name="obj"></param>
	/// <returns></returns>
	Math::Vector3 operator*(const Math::Vector3& obj) const { return Math::Vector3(x * obj.x, y * obj.y, z * obj.z); }

	// メンバー関数としてのスカラー乗算 (Math::Vector3 * float)
	[[nodiscard]] constexpr Math::Vector3 operator*(float scalar) const {
		return Math::Vector3(x * scalar, y * scalar, z * scalar);
	}

	// フリー関数としてのスカラー乗算 (float * Math::Vector3)
	[[nodiscard]] friend constexpr Math::Vector3 operator*(float scalar, const Math::Vector3& vec) {
		return Math::Vector3(vec.x * scalar, vec.y * scalar, vec.z * scalar);
	}

	/// <summary>
	/// 除算
	/// </summary>
	/// <param name="obj"></param>
	/// <returns></returns>
	Math::Vector3 operator/(const Math::Vector3& obj) const { return Math::Vector3(x / obj.x, y / obj.y, z / obj.z); }

	// メンバー関数としてのスカラー乗算 (Math::Vector3 / float)
	[[nodiscard]] constexpr Math::Vector3 operator/(float scalar) const {
		return Math::Vector3(x / scalar, y / scalar, z / scalar);
	}

	// フリー関数としてのスカラー乗算 (float * Math::Vector3)
	[[nodiscard]] friend constexpr Math::Vector3 operator/(float scalar, const Math::Vector3& vec) {
		return Math::Vector3(vec.x / scalar, vec.y / scalar, vec.z / scalar);
	}


	Math::Vector3 operator*=(const Math::Vector3& obj) {
		x *= obj.x;
		y *= obj.y;
		z *= obj.z;
		return *this;
	}

	Math::Vector3 operator*=(const float& obj) {
		x *= obj;
		y *= obj;
		z *= obj;
		return *this;
	}

	Math::Vector3 operator/(const float& obj) {
		return{
		x / obj,
		y / obj,
		z / obj
		};
	}

	// Matrix
	Math::Vector3 operator*(const Math::Matrix4x4& mat) {
		Math::Vector3 result{};

		result.x = mat.m[0][0] * x + mat.m[1][0] * y + mat.m[2][0] * z + mat.m[3][0];
		result.y = mat.m[0][1] * x + mat.m[1][1] * y + mat.m[2][1] * z + mat.m[3][1];
		result.z = mat.m[0][2] * x + mat.m[1][2] * y + mat.m[2][2] * z + mat.m[3][2];

		return result;
	}

	/// <summary>
	/// 正規化
	/// </summary>
	/// <returns></returns>
	Math::Vector3 Normalize() const;

	/// <summary>
	/// 長さをとる
	/// </summary>
	/// <returns></returns>
	float Length() const;

	/// <summary>
	/// ベクトルの2乗の長さを返す
	/// </summary>
	/// <returns></returns>
	float LengthSquared() const;

	/// <summary>
	/// 範囲内に収める
	/// </summary>
	/// <param name="min"></param>
	/// <param name="max"></param>
	/// <returns></returns>
	void Clamp(const Math::Vector3& min, const Math::Vector3& max);

	/// <summary>
	/// 拡縮行列の作成
	/// </summary>
	/// <returns></returns>
	Math::Matrix4x4 MakeScaleMat() const;

	/// <summary>
	/// 回転行列の作成
	/// </summary>
	/// <returns></returns>
	Math::Matrix4x4 MakeRotateMat() const;

	/// <summary>
	/// 平行行列の作成
	/// </summary>
	/// <returns></returns>
	Math::Matrix4x4 MakeTranslateMat() const;

	/// <summary>
	/// 内積
	/// </summary>
	/// <param name="v1">: ベクトル1</param>
	/// <param name="v2">: ベクトル2</param>
	/// <returns></returns>
	static float Dot(const Math::Vector3& v1, const Math::Vector3& v2);

	/// <summary>
	/// 外積
	/// </summary>
	/// <param name="v1">: ベクトル1</param>
	/// <param name="v2">: ベクトル2</param>
	/// <returns></returns>
	static Math::Vector3 Cross(const Math::Vector3& v1, const Math::Vector3& v2);

	/// <summary>
	/// 2つのベクトルから角度を求める
	/// </summary>
	/// <param name="v1"></param>
	/// <param name="v2"></param>
	/// <returns></returns>
	static float AngleBetween(const Math::Vector3& v1, const Math::Vector3& v2);

	/// <summary>
	/// 線形補完
	/// </summary>
	/// <param name="start">: 開始</param>
	/// <param name="end">: 終わり</param>
	/// <param name="t">: 補完係数</param>
	/// <returns></returns>
	static Math::Vector3 Lerp(const Math::Vector3& start, const Math::Vector3& end, float t);

	static Math::Vector3 Min(const Math::Vector3& min, const Math::Vector3& target);
	static Math::Vector3 Max(const Math::Vector3& max, const Math::Vector3& target);

	/// <summary>
	/// Matrixを作成する(z軸)
	/// </summary>
	/// <param name="z"></param>
	/// <returns></returns>
	Math::Matrix4x4 MakeRollMatrix(float z_Roll) const;

	/// <summary>
	/// Matrixを作成する(x軸)
	/// </summary>
	/// <param name="x"></param>
	/// <returns></returns>
	Math::Matrix4x4 MakePitchMatrix(float x_Picth) const;

	/// <summary>
	/// Matrixを作成する(y軸)
	/// </summary>
	/// <param name="y"></param>
	/// <returns></returns>
	Math::Matrix4x4 MakeYawMatrix(float y_Yaw) const;
};

}

}

#ifndef AOENGINE_MATH_NAMESPACE_ALIAS
#define AOENGINE_MATH_NAMESPACE_ALIAS
namespace Math = AOENGINE::Math;
#endif


/// <summary>
/// Math::Vector3定数
/// </summary>
namespace CVector3 {
	constexpr Math::Vector3 RIGHT		{ 1.0f, 0.0f, 0.0f };
	constexpr Math::Vector3 UP		{ 0.0f, 1.0f, 0.0f };
	constexpr Math::Vector3 FORWARD	{ 0.0f ,0.0f, 1.0f };
	constexpr Math::Vector3 ZERO		{ 0.0f, 0.0f, 0.0f };
	constexpr Math::Vector3 UNIT		{ 1.0f, 1.0f, 1.0f };
	constexpr Math::Vector3 INF		{ std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity() };
}