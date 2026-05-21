#pragma once
// c++
#include <vector>
#include <string>
#include <unordered_map>
#include <memory>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
// engine
#include "Engine/DirectX/Pipeline/Pipeline.h"
#include "Engine/System/Asset/AssetHandle.h"
#include "Engine/Module/Components/Meshes/Mesh.h"
#include "Engine/Module/Components/Materials/MaterialStructures.h"
#include "Engine/Module/Components/Materials/BaseMaterial.h"
#include "Engine/Module/Components/Rigging/SkinCluster.h"
#include "Engine/Module/Components/WorldTransform.h"
#include "Engine/Module/Components/ViewProjection.h"
#include "Engine/Module/Components/Animation/AnimationStructures.h"
#include "Engine/Lib/Math/MathStructures.h"

namespace AOENGINE {

class Material;
class PBRMaterial;

/// <summary>
/// モデルクラス
/// </summary>
class Model {
public: // data

	/// <summary>
	/// AnimationのNode
	/// </summary>
	struct NodeAnimationData {
		std::vector<NodeAnimation> animations;
		float tickPerSecond;	// 一秒間に何回の処理が行われるか
		float duration;			// tickPerSecondの持続時間
		float animationTime;	// animationをする時間
	};

	/// <summary>
	/// Node構造体
	/// </summary>
	struct Node {
		Math::QuaternionSRT transform;
		Math::Matrix4x4 localMatrix;				 // NodeのLocalMatrix
		std::string name;					 // Nodeの名前
		std::vector<Node> children;			 // 子供のNode
		NodeAnimationData animationsData;	 // ノードに関するアニメーション
	};

public: // constructor

	Model();
	~Model();

public: // public method

	// 初期化処理
	void Init(ID3D12Device* device, const std::string& directorPath, const std::string& fileName);

	void Debug_Gui(const std::string& name);

	/// <summary>
	/// assimpでのNode解析
	/// </summary>
	/// <param name="node"></param>
	/// <returns></returns>
	Node ReadNode(aiNode* node, const aiScene* scene);

public: // accessor

	const std::string& GetRootNodeName() const { return rootNode_.name; }

	Node& GetNode() { return rootNode_; }

	const std::map<std::string, JointWeightData>& GetSkinClustersData(uint32_t index) { return skinClusterArray_[index]->GetSkinClustersData(); }

	std::unordered_map<std::string, ModelMaterialData>& GetMaterialData() { return materialData_; }

	AOENGINE::Mesh* GetMesh(const uint32_t& index);
	size_t GetMeshsNum() const { return meshArray_.size(); }

	const size_t GetMaterialsSize() const { return materialData_.size(); }

	const std::vector<std::shared_ptr<AOENGINE::Mesh>>& GetMeshArray() const { return meshArray_; }

	std::string GetName() const { return name_; }

	const AssetHandle& GetAssetHandle() const { return assetHandle_; }

private: // private variable

	// 頂点バッファやインデックスバッファを持つ
	std::vector<std::shared_ptr<AOENGINE::Mesh>> meshArray_;
	// materialの情報
	std::unordered_map<std::string, ModelMaterialData> materialData_;
	// skinningのデータ
	std::vector<std::unique_ptr<SkinCluster>> skinClusterArray_;
	// ノード
	Node rootNode_;
	// モデルの名前
	std::string name_;
	// アセットハンドル
	AssetHandle assetHandle_ = AssetHandle(AssetType::Model);
};
}