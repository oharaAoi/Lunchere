#pragma once
#include <string>
#include "Engine/DirectX/Pipeline/Graphics/Object3dPipelines.h"
#include "Engine/DirectX/Pipeline/Graphics/SpritePipelines.h"
#include "Engine/DirectX/Pipeline/Graphics/PrimitivePipeline.h"
#include "Engine/DirectX/Pipeline/PostProcess/ProcessedScenePipelines.h"
#include "Engine/DirectX/Pipeline/Parts/InputLayout.h"

enum class PSOType{
	Object3d,
	Sprite,
	ProcessedScene,
	Primitive,
};

namespace AOENGINE {

/// <summary>
/// 描画のパイプラインをまとめたクラス
/// </summary>
class GraphicsPipelines {
public:

	/// <summary>
	/// ファイルパスの種類
	/// </summary>
	struct AssetPaths {
		std::string object;
		std::string sprite;
		std::string postProcess;
		std::string primitive;
	};

public: // コンストラクタ

	GraphicsPipelines();
	~GraphicsPipelines();

public :

	/// <summary>
	/// 終了処理
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
	/// <param name="path">: ファイルパス</param>
	/// <param name="type">; パイプラインのタイプ</param>
	void Load(const std::string& _path, PSOType _type);

	/// <summary>
	/// 指定したjsonファイルを読み込む
	/// </summary>
	/// <param name="directory">: ディレクトリパス</param>
	/// <param name="fileName">: ファイル名</param>
	/// <param name="type">: パイプラインのタイプ</param>
	void LoadFile(const std::string& _directory, const std::string& _fileName, PSOType _type);

	/// <summary>
	/// jsonデータ読み込み
	/// </summary>
	/// <param name="directory">: ディレクトリパス</param>
	/// <param name="fileName">: ファイル名</param>
	/// <returns></returns>
	json LoadJson(const std::string& _directory, const std::string& _fileName);

	/// <summary>
	/// パイプラインの設定
	/// </summary>
	/// <param name="commandList">: コマンドリスト</param>
	/// <param name="type">: タイプ</param>
	/// <param name="typeName">: パイプラインのファイル名</param>
	void SetPipeline(ID3D12GraphicsCommandList* _commandList, PSOType _type, const std::string& _typeName);

	/// <summary>
	/// 最後に使用したパイプラインのポインタを返す
	/// </summary>
	/// <returns></returns>
	Pipeline* GetLastUsedPipeline() const { return lastUsedPipeline_; }

private:

	const AssetPaths kEngineAssets = {
		"./Project/Packages/Engine/Pipeline/Object/",
		"./Project/Packages/Engine/Pipeline/Sprite/",
		"./Project/Packages/Engine/Pipeline/PostProcess/",
		"./Project/Packages/Engine/Pipeline/Primitive/",
	};

	const AssetPaths kGameAssets = {
		"./Project/Packages/Game/Pipeline/Object/",
		"./Project/Packages/Game/Pipeline/Sprite/",
		"./Project/Packages/Game/Pipeline/PostProcess/",
		""
	};

	std::unique_ptr<Object3dPipelines> obj3dPipeline_;
	std::unique_ptr<SpritePipelines> spritePipeline_;
	std::unique_ptr<ProcessedScenePipelines> processedScenePipeline_;
	std::unique_ptr<PrimitivePipeline> primitivePipeline_;

	// inputLayout
	InputLayout inputLayout_;

	// DXCで使う
	DirectXCompiler* dxCompiler_ = nullptr;
	// device
	ID3D12Device* device_ = nullptr;

	Pipeline* lastUsedPipeline_;

};

}