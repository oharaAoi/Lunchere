#include "Engine/Core/Engine.h"
#include "Engine/Lib/Json//JsonItems.h"
#include "Engine/WinApp/WinApp.h"
#include "Engine/DirectX/Pipeline/Compute/ComputeShaderPipelines.h"
#include "Engine/System/Collision/ColliderCollector.h"
#include "Engine/System/Manager/ParticleManager.h"
#include "Engine/System/Manager/AssetsManager.h"
#include "Engine/System/Editor/Window/EditorWindows.h"
#include "Engine/System/Manager/TextureManager.h"
#include "Engine/System/Input/Input.h"
#include "Engine/Render/SceneRenderer.h"
#include "Engine/Module/ComputeShader/BlendTexture.h"
#include "Engine/Module/Components/ProcessedSceneFrame.h"
#include "Engine/Utilities/Logger.h"
#include "Engine/Utilities/DrawUtils.h"
#include "Engine/Render/Render.h"

using namespace AOENGINE;

Engine::Engine() {}

Engine::~Engine() {}

// ======================================================== //
// 無名名前空間で内部リンゲージする
// ======================================================== //
namespace {
	AOENGINE::Render * render_ = nullptr;

	WinApp* winApp_ = nullptr;

#ifdef _DEBUG
	ImGuiManager* imguiManager_ = nullptr;
#endif
	AOENGINE::Input* input_ = nullptr;
	TextureManager* textureManager_ = nullptr;

	GraphicsContext* graphicsCxt_ = nullptr;

	ID3D12Device* dxDevice_ = nullptr;
	ID3D12GraphicsCommandList* dxCmdList_ = nullptr;
	DescriptorHeap* dxHeap_ = nullptr;

	DirectXCommon* dxCommon_ = nullptr;

	GraphicsPipelines* graphicsPipeline_ = nullptr;
	
	RenderTarget* renderTarget_ = nullptr;

	// CS
	std::unique_ptr<ComputeShaderPipelines> computeShaderPipelines_ = nullptr;
	std::unique_ptr<BlendTexture> blendTexture_ = nullptr;
	// audio
	std::unique_ptr<Audio> audio_ = nullptr;
	
	std::unique_ptr<PostProcess> postProcess_;

	std::unique_ptr<Canvas2d> canvas2d_;

	// オフスクリーンレンダリングで生成したTextureを描画するクラス
	std::unique_ptr<ProcessedSceneFrame> processedSceneFrame_ = nullptr;

	EditorWindows* editorWindows_ = nullptr;

	bool isFullScreen_;

	bool runGame_;

	Pipeline* lastUsedPipeline_;
}


//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　初期化処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void Engine::Initialize(uint32_t _backBufferWidth, uint32_t _backBufferHeight, const char* _windowTitle) {
	CoInitializeEx(0, COINIT_MULTITHREADED);

	InitCoreInstances();

	winApp_->CreateGameWindow(_backBufferWidth, _backBufferHeight, _windowTitle);
	
	InitGraphics();

	InitSystem();

#ifdef _DEBUG
	InitImgui();
#endif

	InitResources();
	
#ifdef _DEBUG
	InitEditor();
#endif

	std::vector<RenderTargetType> types;
	types.push_back(RenderTargetType::Object3D_RenderTarget);
	types.push_back(RenderTargetType::MotionVector_RenderTarget);
	AOENGINE::Render::SetRenderTarget(types, dxCommon_->GetDepthHandle());

	// -------------------------------------------------
	// ↓ その他初期化
	// -------------------------------------------------
	isFullScreen_ = false;
	runGame_ = true;
	
	AOENGINE::Logger::Log("Engine Initialize complete!\n");
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　Engineのコアとなる機能のインスタンスを生成する
//////////////////////////////////////////////////////////////////////////////////////////////////

void Engine::InitCoreInstances() {
	winApp_ = WinApp::GetInstance();
	input_ = AOENGINE::Input::GetInstance();
	render_ = AOENGINE::Render::GetInstance();
	editorWindows_ = AOENGINE::EditorWindows::GetInstance();
	textureManager_ = AOENGINE::TextureManager::GetInstance();

	computeShaderPipelines_ = std::make_unique<ComputeShaderPipelines>();
	processedSceneFrame_	= std::make_unique<ProcessedSceneFrame>();
	postProcess_			= std::make_unique<PostProcess>();
	audio_					= std::make_unique<Audio>();
	blendTexture_			= std::make_unique<BlendTexture>();
	canvas2d_				= std::make_unique<AOENGINE::Canvas2d>();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　グラフィック関連のクラスの初期化を行う
//////////////////////////////////////////////////////////////////////////////////////////////////

void Engine::InitGraphics() {
	graphicsCxt_ = AOENGINE::GraphicsContext::GetInstance();
	graphicsCxt_->Init(winApp_, winApp_->sClientWidth, winApp_->sClientHeight);

	dxDevice_	= graphicsCxt_->GetDevice();
	dxCmdList_	= graphicsCxt_->GetCommandList();
	dxHeap_		= graphicsCxt_->GetDxHeap();
	dxCommon_	= graphicsCxt_->GetDxCommon();

	graphicsPipeline_ = graphicsCxt_->GetGraphicsPipeline();
	renderTarget_ = graphicsCxt_->GetRenderTarget();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　System関連のクラスの初期化を行う
//////////////////////////////////////////////////////////////////////////////////////////////////

void Engine::InitSystem() {
	JsonItems* adjust = JsonItems::GetInstance();
	adjust->Init();

	computeShaderPipelines_->Init(dxDevice_, graphicsCxt_->GetDxCompiler());
	input_->Init(winApp_->GetWNDCLASS(), winApp_->GetHwnd());
	render_->Init(dxCmdList_, dxDevice_, graphicsCxt_->GetRenderTarget());
	audio_->Init();
	canvas2d_->Init();
	postProcess_->Init(dxDevice_, dxHeap_, renderTarget_, graphicsCxt_->GetDxResourceManager());
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　Resource関連のクラスの初期化を行う
//////////////////////////////////////////////////////////////////////////////////////////////////

void Engine::InitResources() {
	textureManager_->Init(dxDevice_, dxCmdList_, dxHeap_, graphicsCxt_->GetDxResourceManager());
	processedSceneFrame_->Init(graphicsCxt_->GetDxResourceManager());

	GeometryFactory& geometryFactory = GeometryFactory::GetInstance();
	geometryFactory.Init();

	AssetsManager::GetInstance()->Init();

	blendTexture_->Init(graphicsCxt_->GetDxResourceManager());
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓ ImGuiの初期化を行う
//////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
void Engine::InitImgui() {
	imguiManager_ = ImGuiManager::GetInstance();
	imguiManager_->Init(winApp_->GetHwnd(), dxDevice_, dxCommon_->GetSwapChainBfCount(), dxHeap_->GetSRVHeap());
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　Editorの初期化を行う
//////////////////////////////////////////////////////////////////////////////////////////////////

void Engine::InitEditor() {
	editorWindows_->Init(dxDevice_, dxCmdList_, renderTarget_, dxHeap_);
	editorWindows_->SetProcessedSceneFrame(processedSceneFrame_.get());
	editorWindows_->SetRenderTarget(renderTarget_);
	editorWindows_->SetCanvas2d(canvas2d_.get());

	RegisterEditorWindowSystem();
}
#endif


//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　終了処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void Engine::Finalize() {
	AssetsManager::GetInstance()->Finalize();
	canvas2d_.reset();
	blendTexture_.reset();
	postProcess_->Finalize();
	audio_->Finalize();
	processedSceneFrame_->Finalize();
	computeShaderPipelines_->Finalize();
	render_->Finalize();
	input_->Finalize();

#ifdef _DEBUG
	editorWindows_->Finalize();
	imguiManager_->Finalize();
#endif
	textureManager_->Finalize();
	graphicsCxt_->Finalize();
	winApp_->Finalize();
	CoUninitialize();
}

bool Engine::ProcessMessage() {
	return  winApp_->ProcessMessage();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　フレーム開始時の処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void Engine::BeginFrame() {
	dxCommon_->Begin();
	input_->Update();
	AssetsManager::GetInstance()->Update();

	if (Input::IsTriggerKey(DIK_F11)) {
		isFullScreen_ = !isFullScreen_;
		WinApp::GetInstance()->SetFullScreen(isFullScreen_);
	}

	std::vector<RenderTargetType> types;
	types.push_back(RenderTargetType::Object3D_RenderTarget);
	types.push_back(RenderTargetType::MotionVector_RenderTarget);
	AOENGINE::Render::SetRenderTarget(types, dxCommon_->GetDepthHandle());

	render_->Update();

#ifdef _DEBUG
	imguiManager_->Begin();
	editorWindows_->Begin();
#endif
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　フレーム終了時の処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void Engine::EndFrame() {
#ifdef _DEBUG
	imguiManager_->End();
	imguiManager_->Draw(dxCmdList_);

	editorWindows_->End();
#endif

	render_->ResetShadowMap();

	dxCommon_->End();
	// buffferのサイズを作り変える
	PendingResize();
	audio_->Update();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　1frameの結果を描画する
//////////////////////////////////////////////////////////////////////////////////////////////////

void Engine::RenderFrame() {
	// -------------------------------------------------
	// ↓ 線の描画
	// -------------------------------------------------
	// gameで使用したlineの描画を開始する
	if (editorWindows_->GetColliderDraw()) {
		ColliderCollector::GetInstance()->Draw();
	}

	if (editorWindows_->GetGridDraw()) {
		DrawGrid(render_->GetViewport3D(), render_->GetProjection3D());
	}
	
	AOENGINE::Render::PrimitiveDrawCall();

	// -------------------------------------------------
	// ↓ PostEffectの実行
	// -------------------------------------------------
	BlendFinalRender(Object3D_RenderTarget);

	postProcess_->Execute(dxCmdList_, processedSceneFrame_->GetResource());

	// -------------------------------------------------
	// ↓ PostObject描画
	// -------------------------------------------------
	renderTarget_->TransitionResource(dxCmdList_, Object3D_RenderTarget, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);
	
	std::vector<RenderTargetType> postRenderTypes;
	postRenderTypes.push_back(RenderTargetType::Object3D_RenderTarget);
	AOENGINE::Render::SetRenderTarget(postRenderTypes, dxCommon_->GetDepthHandle());
	Engine::SetPipeline(PSOType::Object3d, "PostProcess_Normal.json");
	processedSceneFrame_->Draw(dxCmdList_);
	AOENGINE::SceneRenderer::GetInstance()->PostDraw();
	BlendFinalRender(Object3D_RenderTarget);

	// -------------------------------------------------
	// ↓ Sprite描画
	// -------------------------------------------------
	canvas2d_->Update();

	std::vector<RenderTargetType> types;
	types.push_back(RenderTargetType::Sprite2d_RenderTarget);
	AOENGINE::Render::SetRenderTarget(types, dxCommon_->GetDepthHandle());
	Engine::SetPipeline(PSOType::ProcessedScene, "PostProcess_Normal.json");
	processedSceneFrame_->Draw(dxCmdList_);
	canvas2d_->Draw();

	BlendFinalRender(Sprite2d_RenderTarget);

	// guiの描画
#ifdef _DEBUG
	editorWindows_->Update();
#endif

	
	// -------------------------------------------------
	// ↓ 最終的なSceneの描画
	// -------------------------------------------------
	// swapChainの変更
	dxCommon_->SetSwapChain();

	Engine::SetPipeline(PSOType::ProcessedScene, "PostProcess_Normal.json");
	processedSceneFrame_->Draw(dxCmdList_);

	// -------------------------------------------------
	// ↓ 次Frameの準備
	// -------------------------------------------------
	renderTarget_->TransitionResource(dxCmdList_, Object3D_RenderTarget, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);
	renderTarget_->TransitionResource(dxCmdList_, Sprite2d_RenderTarget, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　最終的に描画するTextureを合成する
//////////////////////////////////////////////////////////////////////////////////////////////////

void Engine::BlendFinalRender(RenderTargetType renderTargetType) {
	// -------------------------------------------------
	// ↓ Resourceの状態を切り替える(obj3D, sprite2D, renderTexture)
	// -------------------------------------------------
	renderTarget_->TransitionResource(
		dxCmdList_,
		renderTargetType,
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE
	);

	// 最終描画のTextureを書き込み可能状態にする
	processedSceneFrame_->TransitionResource(dxCmdList_,
									   D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
									   D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	// -------------------------------------------------
	// ↓ object3Dと最終描画のTextureを合成する
	// -------------------------------------------------
	Engine::SetPipelineCS("BlendTexture.json");
	Pipeline* pso = computeShaderPipelines_->GetLastUsedPipeline();
	blendTexture_->Execute(pso, dxCmdList_, renderTarget_->GetRenderTargetSRVHandle(renderTargetType).handleGPU, processedSceneFrame_->GetUAV());

	// -------------------------------------------------
	// ↓ 映すTextureをpixeslShaderで使えるようにする
	// -------------------------------------------------
	processedSceneFrame_->TransitionResource(dxCmdList_, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

}

bool Engine::WorldToGameImagePos(const Math::Vector3& _worldPos, ImVec2& _outScreenPos) {
	Math::Vector4 clip = Math::Vector4(_worldPos, 1.0f);
	Math::Matrix4x4 view = render_->GetViewport3D();
	Math::Matrix4x4 proj = render_->GetProjection3D();
	clip = clip * view;
	clip = clip * proj;

	if (clip.w == 0.0f) {
		return false;
	}
	// NDC
	float ndcX = clip.x / clip.w;
	float ndcY = clip.y / clip.w;
	float ndcZ = clip.z / clip.w;

	if (ndcZ < 0.0f || ndcZ > 1.0f) {
		return false;
	}

	// NDC → Image Local
	float imageX = (ndcX * 0.5f + 0.5f) * processedSceneFrame_->GetAvailSize().x;
	float imageY = (-ndcY * 0.5f + 0.5f) * processedSceneFrame_->GetAvailSize().y; // ★Y反転重要

	_outScreenPos.x = processedSceneFrame_->GetImagePos().x + imageX;
	_outScreenPos.y = processedSceneFrame_->GetImagePos().y + imageY;
	
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　EditorWindowに登録を行う
//////////////////////////////////////////////////////////////////////////////////////////////////

void Engine::RegisterEditorWindowSystem() {
	editorWindows_->AddObjectWindow(render_->GetLightGroup(), "LightGroup");
	editorWindows_->AddObjectWindow(render_->GetShadowMap(), "ShadowMap");
	editorWindows_->AddObjectWindow(postProcess_.get(), "PostProcess");
	editorWindows_->AddObjectWindow(canvas2d_.get(), "Canvas2d");
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　windowのリサイズが保留中であったら
//////////////////////////////////////////////////////////////////////////////////////////////////

void Engine::PendingResize() {
	if (winApp_->sPendingResize) {
		renderTarget_->Finalize();
		blendTexture_->Finalize();
		processedSceneFrame_->Finalize();
		postProcess_->ClearBuffer();
		render_->GetShadowMap()->Finalize();
#ifdef _DEBUG
		editorWindows_->ClearBuffer();
#endif
		dxCommon_->ResetResource();

		// resourceの本解放
		graphicsCxt_->GetDxResourceManager()->Update();
		dxHeap_->FreeList();

		// 作り直し
		graphicsCxt_->ResizeBuffer();

		processedSceneFrame_->Init(graphicsCxt_->GetDxResourceManager());
		render_->GetShadowMap()->Init();
#ifdef _DEBUG
		editorWindows_->ResizeBuffer();
#endif
		postProcess_->ResizeBuffer(dxDevice_, renderTarget_, graphicsCxt_->GetDxResourceManager());
		blendTexture_->Init(graphicsCxt_->GetDxResourceManager());

		canvas2d_->ResizeSprite();

		winApp_->sPendingResize = false;

	} else {
		graphicsCxt_->GetDxResourceManager()->Update();
		dxHeap_->FreeList();
	}
}

std::unique_ptr<AOENGINE::Model> Engine::CreateModel(const std::string& directoryPath, const std::string& filePath) {
	std::unique_ptr<AOENGINE::Model> model = std::make_unique<AOENGINE::Model>();
	model->Init(dxDevice_, directoryPath, filePath);
	return model;
}

void Engine::ReloadModel(AOENGINE::Model* model, const std::string& directoryPath, const std::string& filePath) {
	if (model == nullptr) {
		return;
	}
	model->Init(dxDevice_, directoryPath, filePath);
}

std::unique_ptr<Skinning> Engine::CreateSkinning(AOENGINE::Skeleton* skeleton, AOENGINE::Model* model, uint32_t index) {
	std::unique_ptr<Skinning> result = std::make_unique<Skinning>();
	result->CreateSkinCluster(dxDevice_, skeleton, model->GetMesh(index), dxHeap_, model->GetSkinClustersData(index));
	return result;
}

Pipeline* Engine::SetPipeline(PSOType type, const std::string& typeName) {
	switch (type) {
	case PSOType::Object3d:
		graphicsPipeline_->SetPipeline(dxCmdList_, type, typeName);
		lastUsedPipeline_ = graphicsPipeline_->GetLastUsedPipeline();
		break;
	case PSOType::Sprite:
		graphicsPipeline_->SetPipeline(dxCmdList_, type, typeName);
		lastUsedPipeline_ = graphicsPipeline_->GetLastUsedPipeline();
		break;
	case PSOType::ProcessedScene:
		graphicsPipeline_->SetPipeline(dxCmdList_, type, typeName);
		lastUsedPipeline_ = graphicsPipeline_->GetLastUsedPipeline();
		break;
	case PSOType::Primitive:
		graphicsPipeline_->SetPipeline(dxCmdList_, type, typeName);
		lastUsedPipeline_ = graphicsPipeline_->GetLastUsedPipeline();
		break;
	default:
		break;
	}

	return lastUsedPipeline_;
}

Pipeline* Engine::GetLastUsedPipeline() {
	return lastUsedPipeline_;
}

Pipeline* Engine::GetLastUsedPipelineCS() {
	return computeShaderPipelines_->GetLastUsedPipeline();
}

void Engine::SetSkinning(Skinning* skinning) {
	Pipeline* pso = computeShaderPipelines_->GetLastUsedPipeline();
	//mesh->SetInitVertex();
	skinning->RunCs(pso, dxCmdList_);
	skinning->EndCS(dxCmdList_);
}

Pipeline* Engine::SetPipelineCS(const std::string& jsonFile) {
	computeShaderPipelines_->SetPipeline(dxCmdList_, jsonFile);
	return computeShaderPipelines_->GetLastUsedPipeline();
}

void Engine::ReloadPipeline(PSOType type, const std::string& directoryPath, const std::string& filePath) {
	graphicsPipeline_->LoadFile(directoryPath, filePath, type);
}

void Engine::ReloadPipelineCS(const std::string& directoryPath, const std::string& filePath) {
	computeShaderPipelines_->LoadFile(directoryPath, filePath);
}

AOENGINE::Canvas2d* Engine::GetCanvas2d() {
	return canvas2d_.get();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　Sound系
//////////////////////////////////////////////////////////////////////////////////////////////////

Audio* Engine::GetAudio() {
	return audio_.get();
}

PostProcess* Engine::GetPostProcess() {
	return postProcess_.get();
}
