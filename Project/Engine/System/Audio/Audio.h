#pragma once
#include <xaudio2.h>
#pragma comment(lib, "xaudio2.lib")
#include <fstream>
#include <wrl.h>
#include <cassert>
#include <list>
#include <vector>
#include "Engine/Utilities/Convert.h"

#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#pragma comment(lib, "Mf.lib")
#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "Mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")

namespace AOENGINE {

// チャンクヘッダ
struct ChunkHeader {
	char id[4];		// チャンク毎のID
	int32_t size;	// チャンクサイズ
};

// RIFFヘッダチャンク
struct RiffHeader {
	ChunkHeader Chunk;	// RIFF
	char type[4];		// WAVE
};

// FMTチャンク
struct FormatChunk {
	ChunkHeader chunk;	// fmt
	WAVEFORMATEX fmt;	// 波形フォーマット
};

// 音声データ
struct SoundData {
	// 波形フォーマット
	WAVEFORMATEX wfex;
	// バッファ先頭アドレス
	std::vector<BYTE> pBuffer;
	// バッファサイズ
	unsigned int bufferSize;
};

/// <summary>
/// 音声データ情報
/// </summary>
struct AudioData {
	SoundData data;
	IXAudio2SourceVoice* pSourceVoice;
};

/// <summary>
/// 再生中の音声情報
/// </summary>
struct PlayingSound {
	IXAudio2SourceVoice* pSourceVoice;
	std::vector<BYTE> buffer;
};

/// <summary>
/// 音を管理するクラス
/// </summary>
class Audio {
public: // データ構造体

public:

	template<typename T>
	using ComPtr = Microsoft::WRL::ComPtr <T>;

public:

	Audio() {};
	~Audio();

	/// <summary>
	/// 初期化
	/// </summary>
	void Init();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 終了処理
	/// </summary>
	void Finalize();

	/// <summary>
	/// 音声データの読み込み
	/// </summary>
	/// <param name="filename"></param>
	/// <returns></returns>
	SoundData SoundLoad(const char* filename);

	SoundData LoadWave(const char* filename);

	SoundData LoadMP3(const wchar_t* filename);

	AudioData LoadAudio(const SoundData& loadAudioData);

	/// <summary>
	/// 音声データの解放
	/// </summary>
	/// <param name="soundData"></param>
	void SoundUnload(SoundData* soundData);

	/// <summary>
	/// サウンドの再生
	/// </summary>
	/// <param name="xAudio2"></param>
	/// <param name="soundData"></param>
	void SoundPlayWave(const SoundData& soundData);

	void PlayAudio(const AudioData& audioData, bool isLoop, float volume, bool checkPlaying = false);

	void SingleShotPlay(const SoundData& loadAudioData, float volume);

	/// <summary>
	/// サウンドの停止
	/// </summary>
	/// <param name="soundData"></param>
	void StopAudio(IXAudio2SourceVoice* pSourceVoice);

	/// <summary>
	/// サウンドの一時停止
	/// </summary>
	/// <param name="soundData"></param>
	void PauseAudio(IXAudio2SourceVoice* pSourceVoice);

	/// <summary>
	/// 一時停止からの再生
	/// </summary>
	/// <param name="soundData"></param>
	void ReStartAudio(IXAudio2SourceVoice* pSourceVoice);

	/// <summary>
	/// 音量の設定
	/// </summary>
	/// <param name="pSourceVoice"></param>
	void SetVolume(IXAudio2SourceVoice* pSourceVoice, float volume);

	/// <summary>
	/// サウンドが再生中かどうか
	/// </summary>
	/// <param name="soundData"></param>
	bool IsPlaying(IXAudio2SourceVoice* pSourceVoice);

	const char* GetFileExtension(const char* filename);

	/// <summary>
	/// ゲーム全体の音を設定
	/// </summary>
	/// <param name="volume"></param>
	static void SetMasterVolume(float volume) { masterVolume_ = volume; }

	/// <summary>
	/// ゲーム全体の音を取得
	/// </summary>
	/// <returns></returns>
	static float GetMasterVolume() { return masterVolume_; }

private:

	ComPtr<IXAudio2> xAudio2_;
	IXAudio2MasteringVoice* masterVoice_;

	std::list<PlayingSound> playingSourceList_;

	static float masterVolume_;
};

}