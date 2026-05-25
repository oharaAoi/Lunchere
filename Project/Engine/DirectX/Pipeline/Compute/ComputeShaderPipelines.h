#pragma once
#include <string>
#include "Engine/DirectX/Pipeline/Compute/ComputeShaderPipeline.h"

namespace AOENGINE {

/// <summary>
/// ComputeShaderPipelineを管理しているクラス
/// </summary>
class ComputeShaderPipelines {
public: // コンストラクタ

	ComputeShaderPipelines() = default;
	~ComputeShaderPipelines() = default;

public:

	/// <summary>
	/// 終了関数
	/// </summary>
	void Finalize();

	/// <summary>
	/// 初期化関数
	/// </summary>
	/// <param name="device">デバイス</param>
	/// <param name="dxCompiler">コンパイラー</param>
	/// <param name="shaders">シェーダーファイルパスをまとめたクラス</param>
	void Init(ID3D12Device* _device, DirectXCompiler* _dxCompiler);

	/// <summary>
	/// 読み込み関数
	/// </summary>
	/// <param name="path">" パス</param>
	void Load(const std::string& _path);

	/// <summary>
	/// 指定したjsonファイルを読み込む
	/// </summary>
	/// <param name="directory">: ディレクトリパス</param>
	/// <param name="fileName">: ファイル名</param>
	void LoadFile(const std::string& _directory, const std::string& _fileName);

	/// <summary>
	/// Jsonファイルを読み込む関数
	/// </summary>
	/// <param name="directory">: ディレクトリパス</param>
	/// <param name="fileName">: ファイル名</param>
	/// <returns></returns>
	json LoadJson(const std::string& _directory, const std::string& _fileName);

public:

	void SetPipeline(ID3D12GraphicsCommandList* _commandList, const std::string& _typeName);

	Pipeline* GetLastUsedPipeline() const { return lastUsedPipeline_; }

private:

	const std::string kEngineAssets = "./Project/Packages/Engine/Pipeline/CS/";
	const  std::string kGameAssets = "./Project/Packages/Game/Pipeline/CS/";

	std::unique_ptr<ComputeShaderPipeline> computeShaderPipeline_;

	// inputLayout
	InputLayout inputLayout_;

	// DXCで使う
	DirectXCompiler* dxCompiler_ = nullptr;
	// device
	ID3D12Device* device_ = nullptr;

	Pipeline* lastUsedPipeline_;
};

}