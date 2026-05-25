#include "GraphicsPipelines.h"
#include <iostream>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;
using namespace AOENGINE;

GraphicsPipelines::GraphicsPipelines() {}
GraphicsPipelines::~GraphicsPipelines() {}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 終了処理
///////////////////////////////////////////////////////////////////////////////////////////////

void GraphicsPipelines::Finalize() {
	obj3dPipeline_.reset();
	spritePipeline_.reset();
	processedScenePipeline_.reset();
	primitivePipeline_.reset();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void GraphicsPipelines::Init(ID3D12Device* _device, DirectXCompiler* _dxCompiler) {
	assert(_device);
	assert(_dxCompiler);
	
	device_ = _device;
	dxCompiler_ = _dxCompiler;
	
	obj3dPipeline_ = std::make_unique<Object3dPipelines>();
	spritePipeline_ = std::make_unique<SpritePipelines>();
	processedScenePipeline_ = std::make_unique<ProcessedScenePipelines>();
	primitivePipeline_ = std::make_unique<PrimitivePipeline>();

	obj3dPipeline_->Init(device_, dxCompiler_);
	spritePipeline_->Init(device_, dxCompiler_);
	processedScenePipeline_->Init(device_, dxCompiler_);
	primitivePipeline_->Init(device_, dxCompiler_);

	// engine用
	Load(kEngineAssets.object, PSOType::Object3d);
	Load(kEngineAssets.sprite, PSOType::Sprite);
	Load(kEngineAssets.postProcess, PSOType::ProcessedScene);
	Load(kEngineAssets.primitive, PSOType::Primitive);
	// game用
	Load(kGameAssets.object, PSOType::Object3d);
	Load(kGameAssets.sprite, PSOType::Sprite);
	Load(kGameAssets.postProcess, PSOType::ProcessedScene);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 読み込み処理
///////////////////////////////////////////////////////////////////////////////////////////////

void GraphicsPipelines::Load(const std::string& _path, PSOType _type) {
	if (_path == "") { return; }
	if (!fs::exists(_path)) {
		std::cerr << "Warning: path not found -> " << _path << std::endl;
		return;
	}

	// パイプラン情報を読み込む
	for (const auto& entry : fs::recursive_directory_iterator(_path)) {
		if (entry.is_regular_file()) {
			std::string ext = entry.path().extension().string();

			if (ext == ".json") {
				std::string directory = entry.path().parent_path().string();
				std::string fileName = entry.path().filename().string();
				LoadFile(directory, fileName, _type);
			}
		}

	}
}

void GraphicsPipelines::LoadFile(const std::string& _directory, const std::string& _fileName, PSOType _type) {
	switch (_type) {
	case PSOType::Object3d:
		obj3dPipeline_->AddPipeline(_fileName, LoadJson(_directory, _fileName));
		break;
	case PSOType::Sprite:
		spritePipeline_->AddPipeline(_fileName, LoadJson(_directory, _fileName));
		break;
	case PSOType::ProcessedScene:
		processedScenePipeline_->AddPipeline(_fileName, LoadJson(_directory, _fileName));
		break;
	case PSOType::Primitive:
		primitivePipeline_->AddPipeline(_fileName, LoadJson(_directory, _fileName));
		break;
	default:
		break;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ jsonからパイプライン情報を読取る
///////////////////////////////////////////////////////////////////////////////////////////////

json GraphicsPipelines::LoadJson(const std::string& _directory, const std::string& _fileName) {
	std::string filePath = _directory + "/" + _fileName;

	// 読み込み用ファイルストリーム
	std::ifstream ifs;
	// ファイルを読み込みように開く
	ifs.open(filePath);

	if (ifs.fail()) {
		std::string message = "not Exist " + filePath + ".json";
		assert(0);
	}

	json root;
	// json文字列からjsonのデータ構造に展開
	ifs >> root;
	// ファイルを閉じる
	ifs.close();

	// 値の追加
	return root;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ パイプラインを設定する
///////////////////////////////////////////////////////////////////////////////////////////////

void GraphicsPipelines::SetPipeline(ID3D12GraphicsCommandList* _commandList, PSOType _type, const std::string& _typeName) {
	switch (_type) {
	case PSOType::Object3d:
		obj3dPipeline_->SetPipeline(_commandList, _typeName);
		lastUsedPipeline_ = obj3dPipeline_->GetLastUsedPipeline();
		break;
	case PSOType::Sprite:
		spritePipeline_->SetPipeline(_commandList, _typeName);
		lastUsedPipeline_ = spritePipeline_->GetLastUsedPipeline();
		break;
	case PSOType::ProcessedScene:
		processedScenePipeline_->SetPipeline(_commandList, _typeName);
		lastUsedPipeline_ = processedScenePipeline_->GetLastUsedPipeline();
		break;
	case PSOType::Primitive:
		primitivePipeline_->SetPipeline(_commandList, _typeName);
		lastUsedPipeline_ = primitivePipeline_->GetLastUsedPipeline();
		break;
	default:
		break;
	}
}