#pragma once
#include <d3d12.h>
// c++
#include <cassert>
#include <vector>
#include <memory>
#include <map>
#include <stack>
#include <filesystem>
#include <optional>
// engine
#include <Externals/DirectXTex/DirectXTex.h>
#include <Externals/DirectXTex/d3dx12.h>
#include "Engine/Utilities/Convert.h"
#include "Engine/DirectX/Utilities/DirectXUtils.h"
#include "Engine/DirectX/Descriptor/DescriptorHeap.h"
#include "Engine/DirectX/Resource/DxResource.h"
#include "Engine/DirectX/Resource/DxResourceManager.h"
#include "Engine/System/Asset/AssetHandle.h"
#include "Engine/Lib/Math/Vector2.h"

namespace AOENGINE {

/// <summary>
/// テクスチャ管理クラス
/// </summary>
class TextureManager {
private:

	/// <summary>
	/// テクスチャパス情報
	/// </summary>
	struct TexturePath {
		std::string directory;
		std::string fileName;
	};

public: // メンバ関数

	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	/// <returns></returns>
	static TextureManager* GetInstance();

public:

	TextureManager() = default;
	~TextureManager() = default;
	TextureManager(const TextureManager&) = delete;
	const TextureManager& operator=(const TextureManager&) = delete;

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="device"></param>
	void Init(ID3D12Device* _dxDevice, ID3D12GraphicsCommandList* _commandList, AOENGINE::DescriptorHeap* _dxHeap, AOENGINE::DxResourceManager* _resourceManager);

	/// <summary>
	/// 終了処理
	/// </summary>
	void Finalize();

	/// <summary>
	/// Stackに入っているTextureをすべて読み込む
	/// </summary>
	void LoadStack(bool _forceReload = false);

	/// <summary>
	/// 
	/// </summary>
	/// <param name="directoryPath"></param>
	/// <param name="filePath"></param>
	bool LoadTextureFile(const std::string& directoryPath, const std::string& filePath, bool _forceReload = false);

	/// <summary>
	/// アセットとしてTextureを読み込む
	/// </summary>
	/// <param name="directoryPath"></param>
	/// <param name="filePath"></param>
	/// <param name="_forceReload"></param>
	bool LoadTextureAsset(const std::string& directoryPath, const std::string& filePath, bool _forceReload = false);

	/// <summary>
	/// Textrueデータを読む
	/// </summary>
	/// <param name="filePath"></param>
	/// <returns></returns>
	bool LoadMipImage(const std::string& directoryPath, const std::string& filePath, DirectX::ScratchImage& outMipImage);

	/// <summary>
	/// TextureResourceにデータを転送する
	/// </summary>
	/// <param name="texture"></param>
	/// <param name="mipImage"></param>
	/// <param name="device"></param>
	/// <param name="commandList"></param>
	/// <returns></returns>
	[[nodiscard]]
	ComPtr<ID3D12Resource> UploadTextureData(
		Microsoft::WRL::ComPtr<ID3D12Resource> texture,
		const DirectX::ScratchImage& mipImage,
		Microsoft::WRL::ComPtr<ID3D12Device> device,
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList);

	/// <summary>
	/// ResourceDescを作成する
	/// </summary>
	/// <param name="metadata"></param>
	/// <returns></returns>
	D3D12_RESOURCE_DESC CreateResourceDesc(const DirectX::TexMetadata& metadata);

	/// <summary>
	/// Textureを選択し名前を返す
	/// </summary>
	/// <param name="filePath"></param>
	/// <returns></returns>
	std::string SelectTexture(const std::string& filePath);

	/// <summary>
	/// textureのpreviewを出す
	/// </summary>
	/// <returns></returns>
	bool PreviewTexture(std::string& _textureName);

	/// <summary>
	/// IDからSpriteを探す
	/// </summary>
	/// <param name="id"></param>
	/// <returns></returns>
	std::string SearchSprite(uint32_t id);

	/// <summary>
	/// nameからAssetHandleを探す
	/// </summary>
	/// <param name="name"></param>
	/// <returns></returns>
	std::optional<AssetHandle> SearchAssetHandle(const std::string& name);

private:

	/// <summary>
	/// Stackに溜まっているTextureをDDSに変換する処理
	/// </summary>
	/// <param name="stack"></param>
	/// <param name="scriptPath"></param>
	/// <param name="outputDDSFolder"></param>
	void ConvertAllTexturesFromStack(std::stack<TexturePath>& stack, const std::wstring& scriptPath, const std::string& outputDDSFolder);

	/// <summary>
	/// DDSファイルを読み込む
	/// </summary>
	/// <param name="folderPath"></param>
	void LoadFileDDS(const std::filesystem::path& folderPath, bool _forceReload = false);

public:


	/// <summary>
	/// textureをコマンドに積む
	/// </summary>
	/// <param name="commandList"></param>
	/// <param name="textureNum"></param>
	void SetGraphicsRootDescriptorTable(ID3D12GraphicsCommandList* commandList, const std::string& filePath, const uint32_t& rootParameterIndex);

	AOENGINE::DxResource* GetResource(const std::string& _textureName);

	uint32_t GetSRVDataIndex() { return static_cast<uint32_t>(textureData_.size()); }

	const Math::Vector2 GetTextureSize(const std::string& filePath);

	const std::vector<std::string>& GetFileNames() const { return fileNames_; }

	const DescriptorHandles& GetDxHeapHandles(const std::string& fileName) const;

	void StackTexture(const std::string& directoryPath, const std::string& filePath);

private:

	/// <summary>
	/// テクスチャデータ情報
	/// </summary>
	struct TextureData {
		AOENGINE::DxResource* resource_ = nullptr;;
		ComPtr<ID3D12Resource> intermediateResource_ = nullptr;
		Math::Vector2 textureSize_;
		AssetHandle assetHandle_;

		TextureData()
			: assetHandle_(AssetType::Texture) {
		}
	};

	std::vector<std::string> fileNames_;

	// TextureDataのコンテナ
	std::unordered_map<std::string, TextureData> textureData_;

	// 読み込み予定のTexture
	std::stack<TexturePath> loadStack_;

	// 生成で使う変数
	ID3D12Device* device_;
	AOENGINE::DescriptorHeap* dxHeap_;
	ID3D12GraphicsCommandList* commandList_;
	AOENGINE::DxResourceManager* resourceManager_;
};

}
