#pragma once
#include "Engine/Lib/Math/Matrix4x4.h"
#include "Engine/Lib/Math/Vector3.h"

namespace Math {

/// <summary>
/// 4次元ベクトル
/// </summary>
class Vector4 final {
public:

	float x;
	float y;
	float z;
	float w;

	Vector4() = default;
	~Vector4() = default;

	Math::Vector4(float x, float y, float z, float w)
		: x(x), y(y), z(z), w(w) {
	}

	Math::Vector4(const Math::Vector3& vec3, float w = 1.0f)
		: x(vec3.x), y(vec3.y), z(vec3.z), w(w) {
	}

	// =============================================
	/// <summary>
	/// 加算
	/// </summary>
	/// <param name="obj"></param>
	/// <returns></returns>
	Math::Vector4 operator+(const Math::Vector4& obj) const { return Math::Vector4(x + obj.x, y + obj.y, z + obj.z, w + obj.w); }

	Math::Vector4 operator+(const float& obj) const { return Math::Vector4(x + obj, y + obj, z + obj, w + obj); }

	Math::Vector4 operator+=(const Math::Vector4& obj) {
		x += obj.x;
		y += obj.y;
		z += obj.z;
		w += obj.w;
		return *this;
	}

	Math::Vector4 operator+=(const float& obj) {
		x += obj;
		y += obj;
		z += obj;
		w += obj;
		return *this;
	}

	/// <summary>
	/// 減算
	/// </summary>
	/// <param name="obj"></param>
	/// <returns></returns>
	Math::Vector4 operator-(const Math::Vector4& obj) const { return Math::Vector4(x - obj.x, y - obj.y, z - obj.z, w - obj.w); }

	Math::Vector4 operator-(const float& obj) const { return Math::Vector4(x - obj, y - obj, z - obj, w - obj); }

	Math::Vector4 operator-=(const Math::Vector4& obj) {
		x -= obj.x;
		y -= obj.y;
		z -= obj.z;
		w -= obj.w;
		return *this;
	}

	Math::Vector4 operator-=(const float& obj) {
		x -= obj;
		y -= obj;
		z -= obj;
		w -= obj;
		return *this;
	}

	/// <summary>
	/// 乗算
	/// </summary>
	/// <param name="obj"></param>
	/// <returns></returns>
	Math::Vector4 operator*(const Math::Vector4& obj) const { return Math::Vector4(x * obj.x, y * obj.y, z * obj.z, w * obj.w); }

	Math::Vector4 operator*(const float& obj) const { return Math::Vector4(x * obj, y * obj, z * obj, w * obj); }

	Math::Vector4 operator*=(const Math::Vector4& obj) {
		x *= obj.x;
		y *= obj.y;
		z *= obj.z;
		w *= obj.w;
		return *this;
	}

	Math::Vector4 operator*=(const float& obj) {
		x *= obj;
		y *= obj;
		z *= obj;
		w *= obj;
		return *this;
	}

	// Matrix
	Math::Vector4 operator*(const Math::Matrix4x4& m) {
		return {
		x * m.m[0][0] + y * m.m[1][0] + z * m.m[2][0] + w * m.m[3][0],
		x * m.m[0][1] + y * m.m[1][1] + z * m.m[2][1] + w * m.m[3][1],
		x * m.m[0][2] + y * m.m[1][2] + z * m.m[2][2] + w * m.m[3][2],
		x * m.m[0][3] + y * m.m[1][3] + z * m.m[2][3] + w * m.m[3][3],
		};
	}

	static Math::Vector4 Lerp(const Math::Vector4& start, const Math::Vector4& end, float t);
};

}