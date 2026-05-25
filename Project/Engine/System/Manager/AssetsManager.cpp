#include "AssetsManager.h"
#include "Engine/Core/Engine.h"
#include "Engine/Lib/Json/JsonItems.h"
#include "Engine/System/Manager/TextureManager.h"
#include "Engine/System/Manager/ModelManager.h"
#include "Engine/System/Audio/SoundDatabase.h"
#include "Engine/Utilities/Logger.h"
#include <algorithm>
#include <cctype>
#include <chrono>
#include <iostream>
#include <filesystem>
#include <thread>

using namespace AOENGINE;
namespace fs = std::filesystem;

namespace {

constexpr std::chrono::milliseconds kAssetWatchInterval(1000);
constexpr std::chrono::milliseconds kAssetWatchDebounce(500);
constexpr int kAssetWatchMaxRetryCount = 3;

std::string ToLower(std::string value) {
	std::transform(value.begin(), value.end(), value.begin(), [](unsigned char c) {
		return static_cast<char>(std::tolower(c));
				   });
	return value;
}

std::string MakeWatchKey(const fs::path& path) {
	std::error_code ec;
	fs::path absolutePath = fs::absolute(path, ec);
	if (ec) {
		absolutePath = path;
	}
	return absolutePath.lexically_normal().string();
}

bool TryGetFileState(const fs::path& path, AssetsManager::FileState& outState) {
	std::error_code ec;
	if (!fs::is_regular_file(path, ec) || ec) {
		return false;
	}

	outState.writeTime = fs::last_write_time(path, ec);
	if (ec) {
		return false;
	}

	outState.fileSize = fs::file_size(path, ec);
	if (ec) {
		return false;
	}

	return true;
}

bool IsSameFileState(const AssetsManager::FileState& lhs, const AssetsManager::FileState& rhs) {
	return lhs.writeTime == rhs.writeTime && lhs.fileSize == rhs.fileSize;
}

}

AssetsManager* AssetsManager::GetInstance() {
	static AssetsManager instance;
	return &instance;
}

AssetsManager::~AssetsManager() {
	Finalize();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　初期化処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void AssetsManager::Init() {
	StopWatchThread();

	LoadTextures(kEngineAssets.textures);
	LoadTextures(kGameAssets.textures);

	AOENGINE::Logger::CommentLog("Loading Models");
	LoadModels(kEngineAssets.models);
	LoadModels(kGameAssets.models);

	AOENGINE::Logger::CommentLog("Loading Sounds");
	LoadSounds(kEngineAssets.sounds);
	LoadSounds(kGameAssets.sounds);

	AOENGINE::TextureManager::GetInstance()->LoadStack();

	BuildWatchRoots();
	SnapshotWatchedFiles();
	StartWatchThread();
}

void AssetsManager::Update() {
	std::queue<WatchEvent> events;
	{
		std::lock_guard<std::mutex> lock(assetEventMutex_);
		std::swap(events, assetEventQueue_);
		queuedPaths_.clear();
	}

	while (!events.empty()) {
		ProcessAssetEvent(events.front());
		events.pop();
	}
}

void AssetsManager::Finalize() {
	StopWatchThread();
	{
		std::lock_guard<std::mutex> lock(assetEventMutex_);
		std::queue<WatchEvent> empty;
		std::swap(assetEventQueue_, empty);
		queuedPaths_.clear();
	}
	knownFiles_.clear();
	pendingWatchEvents_.clear();
	watchRoots_.clear();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　textureを読み込む
//////////////////////////////////////////////////////////////////////////////////////////////////

void AssetsManager::LoadTextures(const std::string& rootPath) {
	if (!fs::exists(rootPath)) {
		std::cerr << "Warning: path not found -> " << rootPath << std::endl;
		return;
	}

	TextureManager* manager = AOENGINE::TextureManager::GetInstance();
	for (const auto& entry : fs::recursive_directory_iterator(rootPath)) {
		std::string ext = entry.path().extension().string();
		ext = ToLower(ext);
		if (ext == ".png" || ext == ".jpg" || ext == ".jpeg") {
			std::string directory = entry.path().parent_path().string();
			std::string fileName = entry.path().filename().string();
			directory += "/";
			manager->StackTexture(directory, fileName);
		} else if (ext == ".dds") {
			std::string directory = entry.path().parent_path().string();
			std::string fileName = entry.path().filename().string();
			directory += "/";
			manager->LoadTextureFile(directory, fileName);
		}
	}

}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　3dモデルを読み込む
//////////////////////////////////////////////////////////////////////////////////////////////////

void AssetsManager::LoadModels(const std::string& rootPath) {
	if (!fs::exists(rootPath)) {
		std::cerr << "Warning: path not found -> " << rootPath << std::endl;
		return;
	}
	for (const auto& entry : fs::recursive_directory_iterator(rootPath)) {

		if (entry.is_regular_file()) {
			std::string ext = entry.path().extension().string();
			ext = ToLower(ext);
			
			if (ext == ".obj" || ext == ".gltf") {
				std::string directory = entry.path().parent_path().string();
				std::string fileName = entry.path().filename().string();
				directory += "/";
				ModelManager::GetInstance()->LoadModel(directory, fileName);
			}
		}

	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　soundを読み込む
//////////////////////////////////////////////////////////////////////////////////////////////////

void AssetsManager::LoadSounds(const std::string& rootPath) {
	if (!fs::exists(rootPath)) {
		std::cerr << "Warning: path not found -> " << rootPath << std::endl;
		return;
	}

	for (const auto& entry : fs::recursive_directory_iterator(rootPath)) {
		if (entry.is_regular_file()) {
			std::string ext = entry.path().extension().string();
			ext = ToLower(ext);
			
			if (ext == ".wav" || ext == ".mp3") {
				std::string directory = entry.path().parent_path().string();
				std::string fileName = entry.path().filename().string();
				directory += "/";
				SoundDatabase::LoadAudio(directory, fileName);
			}
		}

	}
}

void AssetsManager::BuildWatchRoots() {
	watchRoots_.clear();

	auto addRoot = [this](const std::string& path, WatchTarget target, int pipelineType = -1) {
		if (!path.empty()) {
			watchRoots_.push_back(WatchRoot{ path, target, pipelineType });
		}
	};

	addRoot(kEngineAssets.textures, WatchTarget::Texture);
	addRoot(kGameAssets.textures, WatchTarget::Texture);
	addRoot(kEngineAssets.models, WatchTarget::Model);
	addRoot(kGameAssets.models, WatchTarget::Model);
	addRoot(kEngineAssets.sounds, WatchTarget::Sound);
	addRoot(kGameAssets.sounds, WatchTarget::Sound);

	addRoot(kEnginePipelineAssets.object, WatchTarget::GraphicsPipeline, static_cast<int>(PSOType::Object3d));
	addRoot(kEnginePipelineAssets.sprite, WatchTarget::GraphicsPipeline, static_cast<int>(PSOType::Sprite));
	addRoot(kEnginePipelineAssets.postProcess, WatchTarget::GraphicsPipeline, static_cast<int>(PSOType::ProcessedScene));
	addRoot(kEnginePipelineAssets.primitive, WatchTarget::GraphicsPipeline, static_cast<int>(PSOType::Primitive));
	addRoot(kEnginePipelineAssets.compute, WatchTarget::ComputePipeline);

	addRoot(kGamePipelineAssets.object, WatchTarget::GraphicsPipeline, static_cast<int>(PSOType::Object3d));
	addRoot(kGamePipelineAssets.sprite, WatchTarget::GraphicsPipeline, static_cast<int>(PSOType::Sprite));
	addRoot(kGamePipelineAssets.postProcess, WatchTarget::GraphicsPipeline, static_cast<int>(PSOType::ProcessedScene));
	addRoot(kGamePipelineAssets.primitive, WatchTarget::GraphicsPipeline, static_cast<int>(PSOType::Primitive));
	addRoot(kGamePipelineAssets.compute, WatchTarget::ComputePipeline);

	addRoot(kJsonItemsObjectPath, WatchTarget::JsonItem);
	addRoot(kJsonItemsEffectPath, WatchTarget::JsonItem);
}

void AssetsManager::SnapshotWatchedFiles() {
	knownFiles_.clear();
	pendingWatchEvents_.clear();

	for (const WatchRoot& root : watchRoots_) {
		std::error_code ec;
		if (!fs::exists(root.path, ec) || ec) {
			continue;
		}

		fs::recursive_directory_iterator it(root.path, fs::directory_options::skip_permission_denied, ec);
		fs::recursive_directory_iterator end;
		for (; it != end; it.increment(ec)) {
			if (ec) {
				ec.clear();
				continue;
			}

			const fs::path& filePath = it->path();
			if (!IsWatchTargetFile(filePath, root.target)) {
				continue;
			}

			FileState state{};
			if (TryGetFileState(filePath, state)) {
				knownFiles_[MakeWatchKey(filePath)] = state;
			}
		}
	}
}

void AssetsManager::StartWatchThread() {
	if (isWatching_.load()) {
		return;
	}

	isWatching_ = true;
	watchThread_ = std::thread(&AssetsManager::WatchLoop, this);
}

void AssetsManager::StopWatchThread() {
	isWatching_ = false;
	if (watchThread_.joinable()) {
		watchThread_.join();
	}
}

void AssetsManager::WatchLoop() {
	while (isWatching_.load()) {
		ScanWatchedFiles();
		std::this_thread::sleep_for(kAssetWatchInterval);
	}
}

void AssetsManager::ScanWatchedFiles() {
	const auto now = std::chrono::steady_clock::now();

	for (const WatchRoot& root : watchRoots_) {
		std::error_code ec;
		if (!fs::exists(root.path, ec) || ec) {
			continue;
		}

		fs::recursive_directory_iterator it(root.path, fs::directory_options::skip_permission_denied, ec);
		fs::recursive_directory_iterator end;
		for (; it != end; it.increment(ec)) {
			if (ec) {
				ec.clear();
				continue;
			}

			const fs::path& filePath = it->path();
			if (!IsWatchTargetFile(filePath, root.target)) {
				continue;
			}

			FileState state{};
			if (!TryGetFileState(filePath, state)) {
				continue;
			}

			std::string key = MakeWatchKey(filePath);
			auto knownIt = knownFiles_.find(key);
			if (knownIt == knownFiles_.end() || !IsSameFileState(knownIt->second, state)) {
				knownFiles_[key] = state;
				pendingWatchEvents_[key] = PendingWatchEvent{
					WatchEvent{ filePath, root.target, root.pipelineType },
					state,
					now
				};
			}
		}
	}

	for (auto it = pendingWatchEvents_.begin(); it != pendingWatchEvents_.end();) {
		auto knownIt = knownFiles_.find(it->first);
		if (knownIt == knownFiles_.end() || !IsSameFileState(knownIt->second, it->second.state)) {
			it = pendingWatchEvents_.erase(it);
			continue;
		}

		if (now - it->second.detectedAt >= kAssetWatchDebounce) {
			EnqueueAssetEvent(it->second.event);
			it = pendingWatchEvents_.erase(it);
		} else {
			++it;
		}
	}
}

void AssetsManager::EnqueueAssetEvent(const WatchEvent& event) {
	std::string key = MakeWatchKey(event.filePath);
	std::lock_guard<std::mutex> lock(assetEventMutex_);
	if (queuedPaths_.contains(key)) {
		return;
	}

	assetEventQueue_.push(event);
	queuedPaths_.insert(key);
}

void AssetsManager::ProcessAssetEvent(const WatchEvent& event) {
	switch (event.target) {
	case WatchTarget::Texture:
		if (!LoadTextureAsset(event.filePath, true) && event.retryCount < kAssetWatchMaxRetryCount) {
			WatchEvent retryEvent = event;
			retryEvent.retryCount++;
			EnqueueAssetEvent(retryEvent);
		}
		break;
	case WatchTarget::Model:
		LoadModelAsset(event.filePath, true);
		break;
	case WatchTarget::Sound:
		LoadSoundAsset(event.filePath, true);
		break;
	case WatchTarget::GraphicsPipeline:
		LoadGraphicsPipelineAsset(event.filePath, event.pipelineType);
		break;
	case WatchTarget::ComputePipeline:
		LoadComputePipelineAsset(event.filePath);
		break;
	case WatchTarget::JsonItem:
		LoadJsonItemAsset(event.filePath);
		break;
	default:
		break;
	}
}

bool AssetsManager::LoadTextureAsset(const std::filesystem::path& filePath, bool forceReload) {
	return AOENGINE::TextureManager::GetInstance()->LoadTextureAsset(
		filePath.parent_path().string() + "/",
		filePath.filename().string(),
		forceReload
	);
}

void AssetsManager::LoadModelAsset(const std::filesystem::path& filePath, bool forceReload) {
	ModelManager::GetInstance()->LoadModel(
		filePath.parent_path().string() + "/",
		filePath.filename().string(),
		forceReload
	);
}

void AssetsManager::LoadSoundAsset(const std::filesystem::path& filePath, bool forceReload) {
	SoundDatabase::LoadAudio(
		filePath.parent_path().string() + "/",
		filePath.filename().string(),
		forceReload
	);
}

void AssetsManager::LoadGraphicsPipelineAsset(const std::filesystem::path& filePath, int pipelineType) {
	if (pipelineType < 0) {
		return;
	}
	Engine::ReloadPipeline(
		static_cast<PSOType>(pipelineType),
		filePath.parent_path().string(),
		filePath.filename().string()
	);
}

void AssetsManager::LoadComputePipelineAsset(const std::filesystem::path& filePath) {
	Engine::ReloadPipelineCS(
		filePath.parent_path().string(),
		filePath.filename().string()
	);
}

void AssetsManager::LoadJsonItemAsset(const std::filesystem::path& filePath) {
	fs::path groupPath = filePath.parent_path();
	fs::path rootPath = groupPath.parent_path();
	std::string rootName = rootPath.filename().string();
	if (rootName != "Object" && rootName != "Effect") {
		return;
	}

	JsonItems::Load(groupPath.filename().string(), filePath.stem().string(), rootPath.string() + "/");
}

bool AssetsManager::IsWatchTargetFile(const std::filesystem::path& filePath, WatchTarget target) const {
	std::string extension = ToLower(filePath.extension().string());

	switch (target) {
	case WatchTarget::Texture:
		return extension == ".png" || extension == ".jpg" || extension == ".jpeg" || extension == ".dds";
	case WatchTarget::Model:
		return extension == ".obj" || extension == ".gltf";
	case WatchTarget::Sound:
		return extension == ".wav" || extension == ".mp3";
	case WatchTarget::GraphicsPipeline:
	case WatchTarget::ComputePipeline:
	case WatchTarget::JsonItem:
		return extension == ".json";
	default:
		return false;
	}
}
