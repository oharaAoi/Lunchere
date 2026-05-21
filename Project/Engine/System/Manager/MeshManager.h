#pragma once
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include "Engine/Module/Components/Meshes/Mesh.h"

namespace AOENGINE {

/// <summary>
/// メッシュ管理クラス
/// </summary>
class MeshManager {
public:

	/// <summary>
	/// メッシュ名とデータの組み合わせ
	/// </summary>
	struct MeshPair {
		std::string meshName;
		std::shared_ptr<AOENGINE::Mesh> mesh;

		MeshPair(const std::string& name, std::shared_ptr<AOENGINE::Mesh> pMesh) {
			meshName = name;
			mesh = std::move(pMesh);
		}
	};

	/// <summary>
	/// メッシュ配列情報
	/// </summary>
	struct MeshArray {
		std::vector<MeshPair> meshArray;
	};

public:

	MeshManager() = default;
	~MeshManager();
	MeshManager(const MeshManager&) = delete;
	const MeshManager& operator=(const MeshManager&) = delete;

	static MeshManager* GetInstance();

public:

	// 初期化処理
	void Init();
	// 終了処理
	void Finalize();

	/// <summary>
	/// Meshの追加
	/// </summary>
	/// <param name="device"></param>
	/// <param name="modelName"></param>
	/// <param name="meshName"></param>
	/// <param name="vertexData"></param>
	/// <param name="indices"></param>
	static void AddMesh(ID3D12Device* device, const std::string& modelName,
						const std::string& meshName, const std::vector<VertexData>& vertexData, const std::vector<uint32_t>& indices);
	/// <summary>
	/// MultiMeshの取得
	/// </summary>
	/// <param name="modelName"></param>
	/// <returns></returns>
	static std::vector<std::shared_ptr<AOENGINE::Mesh>> GetMeshes(const std::string& modelName);

	/// <summary>
	/// Meshが存在するかチェック
	/// </summary>
	/// <param name="modelName"></param>
	/// <returns></returns>
	bool ExistMesh(const std::string& modelName);

	/// <summary>
	/// Mesh単体の取得
	/// </summary>
	/// <param name="meshName"></param>
	/// <returns></returns>
	std::shared_ptr<AOENGINE::Mesh> GetMesh(const std::string& meshName);

	/// <summary>
	/// Meshを選択する
	/// </summary>
	/// <returns></returns>
	std::string SelectMeshName();

private:

	static std::unordered_map<std::string, MeshArray> meshArrayMap_;
	static std::unordered_map<std::string, std::shared_ptr<AOENGINE::Mesh>> meshMap_;
	static std::vector<std::string> modelNameList_;
	static std::vector<std::string> meshNameList_;

};

}