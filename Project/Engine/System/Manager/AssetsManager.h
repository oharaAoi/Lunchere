#pragma once
#include <string>

namespace AOENGINE {

/// <summary>
/// Asset関連を管理してくるクラス
/// </summary>
class AssetsManager final {
public:

	/// <summary>
	/// アセットパス情報
	/// </summary>
	struct AssetPaths {
		std::string textures;
		std::string models;
		std::string sounds;
	};

public:

	AssetsManager() = default;
	~AssetsManager() = default;

	AssetsManager(const AssetsManager&) = delete;
	const AssetsManager& operator=(const AssetsManager&) = delete;

	static AssetsManager* GetInstance();

	void Init();

private:

	// Textureの読み込み
	void LoadTextures(const std::string& rootPath);

	// modelの読み込み
	void LoadModels(const std::string& rootPath);

	// soundの読み込み
	void LoadSounds(const std::string& rootPath);

private:

	const AssetPaths kEngineAssets = {
		"./Project/Packages/Engine/Load/Textures/",
		"./Project/Packages/Engine/Load/Models/",
		"./Project/Packages/Engine/Load/Sounds/"
	};

	const AssetPaths kGameAssets = {
		"./Project/Packages/Game/Load/Textures/",
		"./Project/Packages/Game/Load/Models/",
		"./Project/Packages/Game/Load/Sounds/"
	};
};

}