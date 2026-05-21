#pragma once
#include "Engine/Lib/Math/Vector3.h"

namespace Math {

/// <summary>
/// 3x3行列クラス
/// </summary>
class Matrix3x3 final {
public:
	float m[3][3];

	//3x3の行列の積
	static Math::Matrix3x3 Multiply(const Math::Matrix3x3& matrix1, const Math::Matrix3x3& matrix2);

	void SetRow(int row, const Math::Vector3& v) {
		m[row][0] = v.x;
		m[row][1] = v.y;
		m[row][2] = v.z;
	}
};

}