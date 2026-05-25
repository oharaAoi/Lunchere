#include "ModelManager.h"
#include "Engine/Utilities/Logger.h"
#include "Engine/System/Manager/MeshManager.h"

using namespace AOENGINE;

ModelManager::~ModelManager() {}

ModelManager* ModelManager::GetInstance() {
	static ModelManager instance;
	return &instance;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 終了処理
///////////////////////////////////////////////////////////////////////////////////////////////

void ModelManager::Finalize() {
	modelMap_.clear();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ モデルの読み込み
///////////////////////////////////////////////////////////////////////////////////////////////

void ModelManager::LoadModel(const std::string& directoryPath, const std::string& modelName, bool _forceReload) {
	if (auto it = modelMap_.find(modelName); it != modelMap_.end()) {
		if (_forceReload) {
			MeshManager::GetInstance()->RemoveMeshes(modelName);
			modelPathMap_[modelName] = directoryPath;
			Engine::ReloadModel(it->second.get(), directoryPath, modelName);
		}
		return;
	}
	modelPathMap_.try_emplace(modelName, directoryPath);
	modelNameList_.push_back(modelName);
	modelMap_.try_emplace(modelName, Engine::CreateModel(directoryPath, modelName));
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ IDからモデルを探す
///////////////////////////////////////////////////////////////////////////////////////////////

AOENGINE::Model* AOENGINE::ModelManager::SearchModel(uint32_t id) {
	for (auto& [name, model] : modelMap_) {
		if (model->GetAssetHandle().id == id) {
			return model.get();
		}
	}

	AOENGINE::Logger::AssertLog(std::string("Not Found Model ID\n"));
	return nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　NameからAssetHandleを探す
//////////////////////////////////////////////////////////////////////////////////////////////////

std::optional<AssetHandle> AOENGINE::ModelManager::SearchAssetHandle(const std::string& name) {
	for (auto& [modelName, model] : modelMap_) {
		if (modelName == name) {
			return model->GetAssetHandle();
		}
	}
	AOENGINE::Logger::AssertLog(std::string("Not Found Texture ID\n"));
	return std::nullopt;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 名前からモデルを返す
///////////////////////////////////////////////////////////////////////////////////////////////

AOENGINE::Model* ModelManager::GetModel(const std::string& modelName) {
	if (auto it = modelMap_.find(modelName); it == modelMap_.end()) {
		AOENGINE::Logger::Log(std::string("Not Found : " + modelName + "\n"));
		assert(false && "Model not found!");
	}

	return modelMap_[modelName].get();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ モデルファイルがあるパスを返す
///////////////////////////////////////////////////////////////////////////////////////////////

std::string AOENGINE::ModelManager::GetModelPath(const std::string& modelName) {
	if (auto it = modelPathMap_.find(modelName); it == modelPathMap_.end()) {
		AOENGINE::Logger::Log(std::string("Not Found Path: " + modelName + "\n"));
		assert(false && "Model not found Path!");
	}

	return modelPathMap_[modelName];
}
