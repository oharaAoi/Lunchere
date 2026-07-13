#pragma once
#include "Engine/Lib/Math/Vector2.h"
#include "Engine/Lib/Math/Vector3.h"
#include "Engine/Lib/Math/Vector4.h"

namespace AOENGINE {

/// <summary>
/// 頂点データ構造体
/// </summary>
struct VertexData {
	Math::Vector4 pos;
	Math::Vector2 texcoord;
	Math::Vector3 normal;
	Math::Vector4 tangent;
};

/// <summary>
/// パーティクル用頂点データ
/// </summary>
struct ParticleVertexData {
	Math::Vector4 pos;
	Math::Vector2 texcoord;
};

/// <summary>
/// 三角形頂点情報
/// </summary>
struct TriangleVertices {
	Math::Vector4 vertex1;// 左下
	Math::Vector4 vertex2;// 上
	Math::Vector4 vertex3;// 右下
};

/// <summary>
/// 矩形頂点情報
/// </summary>
struct RectangleVertices {
	Math::Vector4 leftTop		= {-1.0f, -1.0f, 0.0f, 1.0f};
	Math::Vector4 rightTop	= { 1.0f, -1.0f, 0.0f, 1.0f };
	Math::Vector4 leftBottom	= { -1.0f, 1.0f, 0.0f, 1.0f };
	Math::Vector4 rightBottom = { 1.0f, 1.0f, 0.0f, 1.0f };

	void SetSize(const Math::Vector2& sizeRaito){
		leftTop.x *= sizeRaito.x;
		leftTop.y *= sizeRaito.y;

		rightTop.x *= sizeRaito.x;
		rightTop.y *= sizeRaito.y;

		leftBottom.x *= sizeRaito.x;
		leftBottom.y *= sizeRaito.y;

		rightBottom.x *= sizeRaito.x;
		rightBottom.y *= sizeRaito.y;
	}
};

}

using AOENGINE::VertexData;
using AOENGINE::ParticleVertexData;
using AOENGINE::TriangleVertices;
using AOENGINE::RectangleVertices;

