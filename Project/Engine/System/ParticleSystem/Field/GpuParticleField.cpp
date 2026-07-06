#include "GpuParticleField.h"
#include "Engine/Core/Engine.h"
#include "Engine/System/Editor/Window/EditorWindows.h"
#include "Engine/Lib/GameTimer.h"

using namespace AOENGINE;

namespace {
constexpr Math::Vector3 kDefaultAcceleration{ -10.0f, 0.0f, 0.0f };
constexpr Math::Vector3 kDefaultMaxFieldBounds{ 1000.0f, 1000.0f, 1000.0f };
constexpr Math::Vector3 kDefaultMinFieldBounds{ -1000.0f, -1000.0f, -1000.0f };
constexpr UINT kThreadsPerGroup = 256;
}

GpuParticleField::~GpuParticleField() {

}

void GpuParticleField::Debug_Gui() {
	ImGui::DragFloat3("acceleration", &fieldData_->acceleration.x, 0.1f);
	ImGui::DragFloat3("min", &fieldData_->min.x, 0.1f);
	ImGui::DragFloat3("max", &fieldData_->max.x, 0.1f);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void GpuParticleField::Init(uint32_t _instanceNum) {
	AOENGINE::GraphicsContext* ctx = AOENGINE::GraphicsContext::GetInstance();
	ID3D12Device* dxDevice = ctx->GetDevice();

	kInstanceNum_ = _instanceNum;

	fieldBuffer_ = CreateBufferResource(dxDevice, sizeof(AccelerationField));
	fieldBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&fieldData_));

	perFrameBuffer_ = CreateBufferResource(dxDevice, sizeof(PerFrame));
	perFrameBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&perFrame_));

	fieldData_->acceleration = kDefaultAcceleration;
	fieldData_->max = kDefaultMaxFieldBounds;
	fieldData_->min = kDefaultMinFieldBounds;

	isEnable_ = true;

	AOENGINE::EditorWindows::AddObjectWindow(this, "GpuParticleField");
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void GpuParticleField::Update() {
	perFrame_->deltaTime = AOENGINE::GameTimer::DeltaTime();
	perFrame_->time = AOENGINE::GameTimer::TotalTime();
	if (!isEnable_) { return; }
	
	timer_ += AOENGINE::GameTimer::DeltaTime();
}

void GpuParticleField::DrawShape() const {

}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 実行
///////////////////////////////////////////////////////////////////////////////////////////////

void GpuParticleField::Execute(ID3D12GraphicsCommandList* commandList) {
	perFrame_->deltaTime = AOENGINE::GameTimer::DeltaTime();
	perFrame_->time = AOENGINE::GameTimer::TotalTime();

	const UINT groupCount = (kInstanceNum_ + kThreadsPerGroup - 1) / kThreadsPerGroup;


	if (!isEnable_) { return; }
	Engine::SetPipelineCS("AccelerationField.json");
	Pipeline* pso = Engine::GetLastUsedPipelineCS();
	UINT index = 0;
	index = pso->GetRootSignatureIndex("gParticles");
	commandList->SetComputeRootDescriptorTable(index, particleResourceHandle_);
	index = pso->GetRootSignatureIndex("gMaxParticles");
	commandList->SetComputeRootConstantBufferView(index, maxParticleResource_->GetGPUVirtualAddress());
	
	index = pso->GetRootSignatureIndex("gAccelerationField");
	commandList->SetComputeRootConstantBufferView(index, fieldBuffer_->GetGPUVirtualAddress());
	commandList->Dispatch(groupCount, 1, 1);
}
