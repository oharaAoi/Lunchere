#pragma once
#include "Engine/Lib/Math/Vector3.h"
#include "Engine/Lib/Math/Vector4.h"
#include "Engine/Lib/Math/Matrix3x3.h"
#include "Engine/Lib/Math/Matrix4x4.h"

namespace Math {

/// <summary>
/// クォータニオンクラス
/// </summary>
class Quaternion final {
public:

	Math::Quaternion() : x(0.0f), y(0.0f), z(0.0f), w(1.0f) {};
	Math::Quaternion(const float& _x, const float& _y, const float& _z, const float& _w) :
		x(_x), y(_y), z(_z), w(_w) {
	};

	~Quaternion() = default;

	/// <summary>
	/// 正規化した回転を返す
	/// </summary>
	/// <returns>: 回転</returns>
	Math::Quaternion Normalize() const;

	/// <summary>
	/// 行列を返す
	/// </summary>
	/// <returns>: 回転行列</returns>
	Math::Matrix4x4 MakeMatrix() const;

	Math::Vector3 MakeForward() const;
	Math::Vector3 MakeUp() const;
	Math::Vector3 MakeRight() const;

	/// <summary>
	/// Math::Quaternionからオイラー角
	/// </summary>
	/// <returns>: オイラー角</returns>
	Math::Vector3 ToEulerAngles() const;

	/// <summary>
	/// 共役
	/// </summary>
	/// <returns>: w以外の項をx-1したMath::Quaternion</returns>
	Math::Quaternion Conjugate() const;

	/// <summary>
	/// Math::Vector3との掛け算
	/// </summary>
	/// <param name="vec"></param>
	/// <returns>: Math::Vector3と掛け算をしたあとの回転</returns>
	Math::Vector3 Rotate(const Math::Vector3& vec) const;

public:

	/// <summary>
	/// axisの周りをangle度回転するMath::Quaternionを生成する
	/// </summary>
	/// <param name="angle">: 回転させる角度</param>
	/// <param name="axis">: 回転させる軸</param>
	/// <returns></returns>
	static Math::Quaternion AngleAxis(float angle, const Math::Vector3& axis);

	/// <summary>
	/// オイラー角をMath::Quaternionにする
	/// </summary>
	/// <param name="euler">: オイラー角</param>
	/// <returns></returns>
	static Math::Quaternion EulerToQuaternion(const Math::Vector3& euler);

	/// <summary>
	/// Math::Quaternionの逆を返す
	/// </summary>
	/// <param name="rotation"></param>
	/// <returns></returns>
	static Math::Quaternion Inverse(const Math::Quaternion& rotation);

	/// <summary>
	/// from位置からto位置への回転を返す
	/// </summary>
	/// <param name="from">: 現在の位置</param>
	/// <param name="to">: ターゲットの位置</param>
	/// <param name="up">: 回転軸</param>
	/// <returns>: 回転</returns>
	static Math::Quaternion LookAt(const Math::Vector3& from, const Math::Vector3& to, const Math::Vector3& up = Math::Vector3{ 0.0f, 1.0f, 0.0f });

	/// <summary>
	/// from方向からto方向への回転を返す
	/// </summary>
	/// <param name="fromDire">: 元となる方向ベクトル</param>
	/// <param name="toDire">: 向かせたい方向ベクトル</param>
	/// <returns>: to方向へ向く回転</returns>
	static Math::Quaternion FromToRotation(const Math::Vector3& fromDire, const Math::Vector3& toDire);

	/// <summary>
	/// 二つの回転の内積を返す
	/// </summary>
	/// <param name="q1"></param>
	/// <param name="q2"></param>
	/// <returns></returns>
	static float Dot(const Math::Quaternion& q1, const Math::Quaternion& q2);

	/// <summary>
	/// 角度の算出
	/// </summary>
	/// <param name="a"></param>
	/// <param name="b"></param>
	/// <returns></returns>
	static float Angle(Math::Quaternion a, Math::Quaternion b);

	/// <summary>
	/// 球面線形補間
	/// </summary>
	/// <param name="q1"></param>
	/// <param name="q2"></param>
	/// <param name="t"></param>
	/// <returns></returns>
	static Math::Quaternion Slerp(const Math::Quaternion& q1, const Math::Quaternion& q2, const float& t = 0.1f);


	static Math::Quaternion RotateTowards(const Math::Quaternion& q1, const Math::Quaternion& q2, float maxDegreesDelta);

	// オイラー角（ラジアン）をクォータニオンに変換
	static Math::Quaternion EulerToQuaternion(float pitch, float yaw, float roll);

	// クォータニオンをオイラー角（ラジアン）に変換
	Math::Vector3 QuaternionToEuler() const;

	/// <summary>
	/// Math::Quaternionに変換
	/// </summary>
	/// <param name="v"></param>
	/// <returns></returns>
	static Math::Quaternion ToQuaternion(const Math::Vector4& v);

	/// <summary>
	/// 行列からMath::Quaternionを生成する
	/// </summary>
	/// <param name="m">: 行列</param>
	/// <returns>: Math::Quaternion</returns>
	static Math::Quaternion FromMatrix(const Math::Matrix4x4& m);

	static Math::Quaternion RotateMatrixTo(const Math::Matrix3x3 m);

	/// <summary>
	/// 向きたい方向の回転を求める関数
	/// </summary>
	/// <param name="forward"></param>
	/// <param name="upVector"></param>
	/// <returns></returns>
	static Math::Quaternion LookRotation(const Math::Vector3& forward, const Math::Vector3& upVector = CVector3::UP);

	Math::Vector3 RotateVector(const Math::Vector3& v) const;

public:

	Math::Quaternion operator*(const Math::Quaternion& q2) const;

	Math::Vector3 operator*(const Math::Vector3& v) const;

	operator Math::Vector4() const {
		return Math::Vector4(x, y, z, w);
	}

public:

	float x;
	float y;
	float z;
	float w;

};

}