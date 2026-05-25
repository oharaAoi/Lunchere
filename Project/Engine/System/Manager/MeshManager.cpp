#include "MeshManager.h"
#include "Engine/System/Manager/ImGuiManager.h"
#include <algorithm>

using namespace AOENGINE;

std::unordered_map<std::string, MeshManager::MeshArray> MeshManager::meshArrayMap_;
std::unordered_map<std::string, std::shared_ptr<Mesh>> MeshManager::meshMap_;
std::vector<std::string> MeshManager::modelNameList_;
std::vector<std::string> MeshManager::meshNameList_;

MeshManager::~MeshManager() {
}

MeshManager* AOENGINE::MeshManager::GetInstance() {
	static MeshManager instance;
	return &instance;
}

void MeshManager::Init() {

}

void MeshManager::Finalize() {
	meshArrayMap_.clear();
	meshMap_.clear();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ meshの追加
///////////////////////////////////////////////////////////////////////////////////////////////

void MeshManager::AddMesh(ID3D12Device* device, const std::string& modelName, const std::string& meshName,
						  const std::vector<VertexData>& vertexData, const std::vector<uint32_t>& indices) {

	auto it = meshArrayMap_.find(modelName);
	if (it == meshArrayMap_.end()) {
		MeshPair meshPair(meshName, std::make_shared<Mesh>());
		meshPair.mesh->Init(device, vertexData, indices);
		// メッシュを登録
		auto& newMeshs = meshArrayMap_[modelName].meshArray.emplace_back(std::move(meshPair));
		// 名前リストを更新
		modelNameList_.push_back(modelName);

		// 
		meshMap_[meshName] = newMeshs.mesh;
		meshNameList_.push_back(meshName);
	} else {
		MeshPair meshPair(meshName, std::make_shared<Mesh>());
		meshPair.mesh->Init(device, vertexData, indices);
		// メッシュを登録
		meshArrayMap_[modelName].meshArray.push_back(std::move(meshPair));
		// 名前リストを更新
		modelNameList_.push_back(meshName);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ meshの取得
///////////////////////////////////////////////////////////////////////////////////////////////

std::vector<std::shared_ptr<Mesh>> MeshManager::GetMeshes(const std::string& modelName) {
	std::vector<std::shared_ptr<Mesh>> result;

	auto it = meshArrayMap_.find(modelName);
	if (it == meshArrayMap_.end()) {
		assert(false && "Model not found!");
	}

	for (auto& origine : it->second.meshArray) {
		result.emplace_back(std::make_shared<Mesh>(*origine.mesh));
	}

	return result;
}

std::shared_ptr<Mesh> MeshManager::GetMesh(const std::string& meshName) {
	for (auto& meshes : meshArrayMap_) {
		const std::vector<MeshPair>& meshPair = meshes.second.meshArray;
		for (uint32_t oi = 0; oi < meshPair.size(); ++oi) {
			if (meshPair[oi].meshName == meshName) {
				return meshPair[oi].mesh;
			}
		}
	}

	// エラー出力する
	return nullptr;
}


///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ meshがあるかをチェック
///////////////////////////////////////////////////////////////////////////////////////////////

bool MeshManager::ExistMesh(const std::string& modelName) {
	if (meshArrayMap_.find(modelName) == meshArrayMap_.end()) {
		return false;
	} else {
		return true;
	}
}

void MeshManager::RemoveMeshes(const std::string& modelName) {
	auto it = meshArrayMap_.find(modelName);
	if (it == meshArrayMap_.end()) {
		return;
	}

	for (const MeshPair& meshPair : it->second.meshArray) {
		meshMap_.erase(meshPair.meshName);
		std::erase(meshNameList_, meshPair.meshName);
	}

	meshArrayMap_.erase(it);
	std::erase(modelNameList_, modelName);
}

std::string MeshManager::SelectMeshName() {
	static std::string selectedFilename;
	static int selectedIndex = -1; // 選択されたインデックス
	if (ImGui::TreeNode("Meshs")) {
		if (ImGui::BeginListBox("Mesh List")) {
			for (int i = 0; i < meshNameList_.size(); ++i) {
				const bool isSelected = (i == selectedIndex);
				std::string textureName = meshNameList_[i];
				if (ImGui::Selectable(textureName.c_str(), isSelected)) {
					selectedIndex = i;
					selectedFilename = meshNameList_[i];
				}
			}
			ImGui::EndListBox();
		}
		ImGui::TreePop();
	}

	return selectedFilename;
}
