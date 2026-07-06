#include "BaseScene.h"
#include "Engine/System/Editor/Window/EditorWindows.h"
#include "Engine/Lib/Json/JsonItems.h"
#include "Engine/Module/PostEffect/PostProcess.h"
#include "Engine/Render/Render.h"

namespace {
constexpr int kSkyboxRenderQueue = -999;
}

BaseScene::~BaseScene() {
	pSceneRenderer_->Finalize();
	collisionManager_->Finalize();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ sceneの読み込み処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BaseScene::LoadScene(const std::string& directory, const std::string& fileName, const std::string& extension) {
	collisionManager_ = std::make_unique<AOENGINE::CollisionManager>();
	collisionManager_->Init();
	
	pSceneLoader_ = AOENGINE::SceneLoader::GetInstance();
	pSceneLoader_->Init();
	pSceneLoader_->Load(directory, fileName, extension);

	pSceneRenderer_ = AOENGINE::SceneRenderer::GetInstance();
	pSceneRenderer_->Init();
	pSceneRenderer_->CreateObject(pSceneLoader_->GetLevelData());
	AOENGINE::EditorWindows::GetInstance()->SetSceneRenderer(pSceneRenderer_);

	skybox_ = AOENGINE::SceneRenderer::GetInstance()->AddObject<Skybox>("Skybox", "Object_Skybox.json", kSkyboxRenderQueue);
	AOENGINE::Render::SetSkyboxTexture(skybox_->GetTexture());

	AOENGINE::EditorWindows::GetInstance()->AddObjectWindow(pSceneRenderer_, "sceneRenderer");
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ Updateより後に呼び出される更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BaseScene::UpdateProcess() {
	this->Update();
	pSceneRenderer_->Update();
	collisionManager_->CheckAllCollision();

	pSceneRenderer_->PostUpdate();
	this->PostUpdate();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 描画処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BaseScene::Draw() const {
	pSceneRenderer_->Draw();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ シーンを保存する
///////////////////////////////////////////////////////////////////////////////////////////////

void BaseScene::SaveSceneEffect() {
	Engine::GetPostProcess()->Save(this->sceneName_ + "/");
	//Engine::GetCanvas2d()->Save(this->sceneName_);
}

void BaseScene::LoadSceneEffect() {
	std::string path = AOENGINE::JsonItems::GetDirectoryPath() + this->sceneName_ + "/";
	AOENGINE::JsonItems::GetInstance()->LoadDesignationPath(path);
	Engine::GetPostProcess()->Load(this->sceneName_ + "/");

	//Engine::GetCanvas2d()->Load(this->sceneName_);
}
