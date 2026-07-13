#pragma once
#include <vector>
#include <string>
#include "Engine/Lib/Color.h"
#include "Engine/Lib/Math/MathStructures.h"

namespace AOENGINE {

enum class MaterialType {
	Normal,
	PBR
};

/// <summary>
/// モデルマテリアル情報
/// </summary>
struct ModelMaterialData {
	AOENGINE::Color color = { 1.0f, 1.0f, 1.0f, 1.0f };
	std::string textureFilePath = "white.png";
};

}

using AOENGINE::MaterialType;
using AOENGINE::ModelMaterialData;
