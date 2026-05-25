#include "ComputeShaderPipelines.h"
#include <fstream>
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;
using namespace AOENGINE;

void ComputeShaderPipelines::Finalize() {
	computeShaderPipeline_.reset();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void ComputeShaderPipelines::Init(ID3D12Device* _device, DirectXCompiler* _dxCompiler) {
	assert(_device);
	assert(_dxCompiler);

	device_ = _device;
	dxCompiler_ =_dxCompiler;

	// cs用のパイプラインの作成
	computeShaderPipeline_ = std::make_unique<ComputeShaderPipeline>();
	computeShaderPipeline_->Init(_device, _dxCompiler);
	
	// engine用
	Load(kEngineAssets);
	
	// game用
	Load(kGameAssets);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 読み込み処理
///////////////////////////////////////////////////////////////////////////////////////////////

void ComputeShaderPipelines::Load(const std::string& _path) {
	// パスが存在しないなら早期リターン
	if (!fs::exists(_path)) {
		std::cerr << "Warning: path not found -> " << _path << std::endl;
		return;
	}

	// jsonファイルの読み込み
	for (const auto& entry : fs::recursive_directory_iterator(_path)) {
		if (entry.is_regular_file()) {
			std::string ext = entry.path().extension().string();

			if (ext == ".json") {
				std::string directory = entry.path().parent_path().string();
				std::string fileName = entry.path().filename().string();

				LoadFile(directory, fileName);
			}
		}

	}
}

void ComputeShaderPipelines::LoadFile(const std::string& _directory, const std::string& _fileName) {
	computeShaderPipeline_->AddPipeline(_fileName, LoadJson(_directory, _fileName));
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ jsonからパイプライン情報の読み込み
///////////////////////////////////////////////////////////////////////////////////////////////

json ComputeShaderPipelines::LoadJson(const std::string& _directory, const std::string& _fileName) {
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

void ComputeShaderPipelines::SetPipeline(ID3D12GraphicsCommandList* _commandList, const std::string& _typeName) {
	computeShaderPipeline_->SetPipeline(_commandList, _typeName);
	lastUsedPipeline_ = computeShaderPipeline_->GetLastUsedPipeline();
}