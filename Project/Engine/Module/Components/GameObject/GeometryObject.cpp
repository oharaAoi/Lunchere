#include "GeometryObject.h"
#include "Engine/Render/Render.h"
#include "Engine/Core/Engine.h"
#include "Engine/Core/GraphicsContext.h"
#include "Engine/System/Editor/Window/EditorWindows.h"
#include "Engine/Module/Geometry/Polygon/PlaneGeometry.h"
#include "Engine/Module/Geometry/Polygon/SphereGeometry.h"
#include "Engine/Module/Geometry/Polygon/CubeGeometry.h"
#include "Engine/Module/Geometry/Polygon/RingGeometry.h"
#include "Engine/Module/Geometry/Polygon/CylinderGeometry.h"
#include "Engine/System/Manager/TextureManager.h"
#include "Engine/Module/Components/GameObject/Model.h"

using namespace AOENGINE;

GeometryObject::~GeometryObject() {
	material_.reset();
	transform_->Finalize();
}

void GeometryObject::Update() {
	transform_->Update();
	material_->Update();
}

void GeometryObject::Draw() const {
	ID3D12GraphicsCommandList* commandList = AOENGINE::GraphicsContext::GetInstance()->GetCommandList();
	Pipeline* pso = Engine::GetLastUsedPipeline();

	AOENGINE::Render::DrawLightGroup(pso);
	commandList->IASetVertexBuffers(0, 1, &mesh_->GetVBV());
	commandList->IASetIndexBuffer(&mesh_->GetIBV());

	UINT index = pso->GetRootSignatureIndex("gMaterial");
	commandList->SetGraphicsRootConstantBufferView(index, material_->GetBufferAddress());

	index = pso->GetRootSignatureIndex("gWorldTransformMatrix");
	transform_->BindCommandList(commandList, index);

	index = pso->GetRootSignatureIndex("gViewProjectionMatrix");
	AOENGINE::Render::GetInstance()->GetViewProjection()->BindCommandList(commandList, index);
	index = pso->GetRootSignatureIndex("gViewProjectionMatrixPrev");
	AOENGINE::Render::GetInstance()->GetViewProjection()->BindCommandListPrev(commandList, index);

	index = pso->GetRootSignatureIndex("gTexture");
	std::string textureName = material_->GetAlbedoTexture();
	AOENGINE::TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(commandList, textureName, index);

	index = pso->GetRootSignatureIndex("gShadowMap");
	commandList->SetGraphicsRootDescriptorTable(index, AOENGINE::Render::GetShadowMap()->GetDeptSrvHandle().handleGPU);
	
	commandList->DrawIndexedInstanced(mesh_->GetIndexNum(), 1, 0, 0, 0);
}


void GeometryObject::Debug_Gui() {
	transform_->Debug_Gui();
	material_->Debug_Gui();
}

void GeometryObject::SetEditorWindow() {
	AOENGINE::EditorWindows::AddObjectWindow(this, id_);
}

void GeometryObject::Init() {
	mesh_.reset();
	material_.reset();
	transform_.reset();

	mesh_ = std::make_unique<Mesh>();
	material_ = std::make_unique<Material>();
	material_->Init();
	transform_ = std::make_unique<WorldTransform>();
}