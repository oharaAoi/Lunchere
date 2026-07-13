#pragma once
#include <memory>
#include "Engine/Core/GraphicsContext.h"
#include "Engine/System/Manager/ImGuiManager.h"
#include "Engine/System/Audio/Audio.h"
#include "Engine/Module/Components/GameObject/Model.h"
#include "Engine/Module/Components/2d/Canvas2d.h"
#include "Engine/Module/Components/WorldTransform.h"
#include "Engine/Module/Components/Rigging/Skinning.h"
#include "Engine/Module/PostEffect/PostProcess.h"

namespace AOENGINE {

/// <summary>
/// GameEngine
/// </summary>
class Engine {
public:

	Engine();
	~Engine();

	static void Initialize(uint32_t _backBufferWidth, uint32_t _backBufferHeight, const char* _windowTitle);

	static void Finalize();

public:

	/// <summary>
	/// ループの判定
	/// </summary>
	/// <returns></returns>
	static bool ProcessMessage();

	/// <summary>
	/// フレームを開始する
	/// </summary>
	static void BeginFrame();

	/// <summary>
	/// フレームを終了する
	/// </summary>
	static void EndFrame();

	/// <summary>
	/// Frameを描画する
	/// </summary>
	static void RenderFrame();

	/// <summary>
	/// Textureの合成
	/// </summary>
	/// <param name="renderTargetType"></param>
	static void BlendFinalRender(RenderTargetType renderTargetType);

	/// <summary>
	/// world上の座標をGame画面に合わせた座標で返す
	/// </summary>
	/// <param name="_worldPos"></param>
	/// <param name="_outScreenPos"></param>
	/// <returns></returns>
	static bool WorldToGameImagePos(const Math::Vector3& _worldPos, ImVec2& _outScreenPos);

	/// <summary>
	/// systemとなる要素をEditorWindowに登録する
	/// </summary>
	static void RegisterEditorWindowSystem();

private:

	/// <summary>
	/// bufferのサイズを作り変える
	/// </summary>
	static void PendingResize();

	/// <summary>
	/// Engineのコアとなる機能のインスタンスを生成する
	/// </summary>
	static void InitCoreInstances();

	/// <summary>
	/// グラフィック関連のクラスの初期化を行う
	/// </summary>
	static void InitGraphics();

	/// <summary>
	/// System関連のクラスの初期化を行う
	/// </summary>
	static void InitSystem();

	/// <summary>
	/// Resource関連のクラスの初期化を行う
	/// </summary>
	static void InitResources();

#ifdef _DEBUG
	/// <summary>
	/// ImGuiの初期化を行う
	/// </summary>
	static void InitImgui();

	/// <summary>
	/// Editorの初期化を行う
	/// </summary>
	static void InitEditor();
#endif

public:

	/////////////////////////////////////////////////////////////////////////////////////////////
	// 生成系
	/////////////////////////////////////////////////////////////////////////////////////////////
	// モデルのポインタを作成
	static std::unique_ptr<AOENGINE::Model> CreateModel(const std::string& directoryPath, const std::string& filePath);
	// Modelを再読み込みする
	static void ReloadModel(AOENGINE::Model* model, const std::string& directoryPath, const std::string& filePath);
	// Skinningの作成
	static std::unique_ptr<AOENGINE::Skinning> CreateSkinning(AOENGINE::Skeleton* skeleton, AOENGINE::Model* model, uint32_t index);

	/////////////////////////////////////////////////////////////////////////////////////////////
	// Pipelineの設定
	/////////////////////////////////////////////////////////////////////////////////////////////

	static AOENGINE::Pipeline* SetPipeline(PSOType type, const std::string& typeName);
	static AOENGINE::Pipeline* SetPipelineCS(const std::string& jsonFile);
	static void ReloadPipeline(PSOType type, const std::string& directoryPath, const std::string& filePath);
	static void ReloadPipelineCS(const std::string& directoryPath, const std::string& filePath);

	static AOENGINE::Pipeline* GetLastUsedPipeline();
	static AOENGINE::Pipeline* GetLastUsedPipelineCS();

	static void SetSkinning(AOENGINE::Skinning* skinning);

	/////////////////////////////////////////////////////////////////////////////////////////////
	// Getter/Setter
	/////////////////////////////////////////////////////////////////////////////////////////////

	static AOENGINE::Audio* GetAudio();

	static AOENGINE::Canvas2d* GetCanvas2d();

	static AOENGINE::PostProcess* GetPostProcess();
};

}

using AOENGINE::Engine;
