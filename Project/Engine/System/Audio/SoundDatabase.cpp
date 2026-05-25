#include "SoundDatabase.h"
#include "Engine/Utilities/Logger.h"

using namespace AOENGINE;

SoundDatabase::~SoundDatabase() {
	for (auto& pair : audioLoadData_) {
		pair.second.pBuffer.~vector();
	}
	audioLoadData_.clear();
}

SoundDatabase* SoundDatabase::GetInstance() {
	static SoundDatabase instance;
	return &instance;
}

void SoundDatabase::Init() {
	audioLoadData_.clear();
}

void SoundDatabase::AddMap(const std::string& directoryPath, const std::string& fileName, bool _forceReload) {
	std::string name = directoryPath + fileName;
	AOENGINE::Logger::Log("[Load][Audio] :" + fileName);
	if (auto it = audioLoadData_.find(fileName); it != audioLoadData_.end()) {
		if (!_forceReload) {
			return;
		}
		audioLoadData_.erase(it);
	}
	AOENGINE::Logger::Log(" --- success!\n");

	audioLoadData_.try_emplace(fileName, Engine::GetAudio()->SoundLoad(name.c_str()));
}

SoundData SoundDatabase::GetAudioData(const std::string& fileName) {
	if (auto it = audioLoadData_.find(fileName); it == audioLoadData_.end()) {
		assert(false && "Audio not found!");
	}
	return audioLoadData_[fileName];
}

void SoundDatabase::LoadAudio(const std::string& directoryPath, const std::string& fileName, bool _forceReload) {
	GetInstance()->AddMap(directoryPath, fileName, _forceReload);
}
