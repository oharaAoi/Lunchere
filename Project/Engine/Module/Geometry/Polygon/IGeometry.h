#pragma once
#include <vector>
#include <string>
#include "Engine/Module/Geometry/Structs/Vertices.h"

namespace AOENGINE {

/// <summary>
/// ジオメトリ基底クラス
/// </summary>
class IGeometry {
public:

	IGeometry() = default;
	virtual ~IGeometry() = default;

	const std::vector<VertexData>& GetVertex() const { return vertexData_; }

	const std::vector<uint32_t>& GetIndex() const { return indices_; }

	const std::string& GetGeometryName() { return geometryName_; }

protected:

	std::vector<VertexData> vertexData_;
	std::vector<uint32_t> indices_;

	std::string geometryName_;

};

}