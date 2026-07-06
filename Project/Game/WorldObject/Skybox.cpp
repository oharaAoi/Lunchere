#include "Skybox.h"
#include "Engine/Core/Engine.h"
#include "Engine/Render/Render.h"
#include "Engine/System/Manager/MeshManager.h"
#include "Engine/System/Manager/TextureManager.h"

namespace {
constexpr float kSkyboxScale = 300.0f;
constexpr float kSkyboxRotateY = 180.0f * kPI;
}

Skybox::~Skybox() {
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void Skybox::Init() {
	cube_.Init(Math::Vector3(kSkyboxScale, kSkyboxScale, kSkyboxScale));
	cube_.Inverse();

	// meshの作成
	std::string name = cube_.GetGeometryName();
	if (!ExistMesh(name)) {
		mesh_ = std::make_shared<AOENGINE::Mesh>();
		mesh_->Init(AOENGINE::GraphicsContext::GetInstance()->GetDevice(), cube_.GetVertex(), cube_.GetIndex());
		AddMeshManager(mesh_, name);
	} else {
		mesh_ = AOENGINE::MeshManager::GetInstance()->GetMesh(name);
	}

	// その他の作成
	material_ = std::make_unique<AOENGINE::Material>();
	material_->Init();
	transform_ = std::make_unique<AOENGINE::WorldTransform>();
	transform_->Init();
	transform_->SetRotate(Math::Quaternion::AngleAxis(kSkyboxRotateY, CVector3::UP));
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void Skybox::Update() {
	transform_->Update();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 描画処理
///////////////////////////////////////////////////////////////////////////////////////////////

void Skybox::Draw() const {
	ID3D12GraphicsCommandList* commandList = AOENGINE::GraphicsContext::GetInstance()->GetCommandList();
	AOENGINE::Pipeline* pso = Engine::SetPipeline(PSOType::Object3d, "Object_Skybox.json");
	UINT index = pso->GetRootSignatureIndex("gMaterial");

	commandList->IASetVertexBuffers(0, 1, &mesh_->GetVBV());
	commandList->IASetIndexBuffer(&mesh_->GetIBV());
	commandList->SetGraphicsRootConstantBufferView(index, material_->GetBufferAddress());

	index = pso->GetRootSignatureIndex("gWorldTransformMatrix");
	transform_->BindCommandList(commandList, index);
	index = pso->GetRootSignatureIndex("gViewProjectionMatrix");
	AOENGINE::Render::GetInstance()->GetViewProjection()->BindCommandList(commandList, index);
	index = pso->GetRootSignatureIndex("gViewProjectionMatrixPrev");
	AOENGINE::Render::GetInstance()->GetViewProjection()->BindCommandListPrev(commandList, index);

	index = pso->GetRootSignatureIndex("gTexture");
	AOENGINE::TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(commandList, useTexture_, index);

	commandList->DrawIndexedInstanced(mesh_->GetIndexNum(), 1, 0, 0, 0);
}

void Skybox::AddMeshManager(std::shared_ptr<AOENGINE::Mesh>& _pMesh, const std::string& name) {
	AOENGINE::MeshManager::GetInstance()->AddMesh(AOENGINE::GraphicsContext::GetInstance()->GetDevice(), name, name, _pMesh->GetVerticesData(), _pMesh->GetIndices());
}

bool Skybox::ExistMesh(const std::string& name) {
	return AOENGINE::MeshManager::GetInstance()->ExistMesh(name);
}
