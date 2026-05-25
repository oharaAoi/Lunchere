#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include <assert.h>
#include "Engine/Core/Engine.h"
#include "Engine/Module/Components/GameObject/Model.h"

namespace AOENGINE {

/// <summary>
/// Modelを管理するクラス
/// </summary>
class ModelManager {
public: // constructor

	ModelManager() = default;
	~ModelManager();
	ModelManager(const ModelManager&) = delete;
	const ModelManager& operator=(const ModelManager&) = delete;

	static ModelManager* GetInstance();

public: // public method

	// 終了
	void Finalize();

	/// <summary>
	/// modelを読み込む
	/// </summary>
	/// <param name="directoryPath"></param>
	/// <param name="modelName"></param>
	void LoadModel(const std::string& directoryPath, const std::string& modelName, bool _forceReload = false);

	/// <summary>
	/// idからモデルを探す
	/// </summary>
	/// <param name="id"></param>
	/// <returns></returns>
	AOENGINE::Model* SearchModel(uint32_t id);

	/// <summary>
	/// nameからAssetHandleを探す
	/// </summary>
	/// <param name="name"></param>
	/// <returns></returns>
	std::optional<AssetHandle> SearchAssetHandle(const std::string& name);

public: // public method

	/// <summary>
	/// modelを取得する
	/// </summary>
	/// <param name="modelName"></param>
	/// <returns></returns>
	AOENGINE::Model* GetModel(const std::string& modelName);

	/// <summary>
	/// modelのパスを取得
	/// </summary>
	/// <param name="modelName"></param>
	/// <returns></returns>
	std::string GetModelPath(const std::string& modelName);

	std::vector<std::string>& GetModelNameList() { return modelNameList_; }
	std::vector<std::string>& GetModelNames() { return GetInstance()->GetModelNameList(); }

private: // private variable

	std::unordered_map<std::string, std::unique_ptr<AOENGINE::Model>> modelMap_;
	std::vector<std::string> modelNameList_;

	std::unordered_map<std::string, std::string> modelPathMap_;
};

}