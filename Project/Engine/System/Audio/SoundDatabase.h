#pragma once
#include <map>
#include <string>
#include <assert.h>
#include "Engine/System/Audio/Audio.h"
#include "Engine/Core/Engine.h"

namespace AOENGINE {

/// <summary>
/// soundDataを管理しているクラス
/// </summary>
class SoundDatabase {
public:

	SoundDatabase() = default;
	~SoundDatabase();
	SoundDatabase(const SoundDatabase&) = delete;
	const SoundDatabase& operator=(const SoundDatabase&) = delete;

	static SoundDatabase* GetInstance();

	void Init();

	/// <summary>
	/// Audioをmapに追加する
	/// </summary>
	/// <param name="directoryPath"></param>
	/// <param name="fileName"></param>
	void AddMap(const std::string& directoryPath, const std::string& fileName, bool _forceReload = false);

	/// <summary>
	/// Audioを構成するデータを取得
	/// </summary>
	/// <param name="fileName"></param>
	/// <returns></returns>
	SoundData GetAudioData(const std::string& fileName);

	/// <summary>
	/// Audioを読み込み
	/// </summary>
	/// <param name="directoryPath"></param>
	/// <param name="fileName"></param>
	static void LoadAudio(const std::string& directoryPath, const std::string& fileName, bool _forceReload = false);

private:

	std::map<std::string, const SoundData> audioLoadData_;

};

}