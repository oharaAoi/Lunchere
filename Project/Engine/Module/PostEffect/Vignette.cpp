#include "Vignette.h"
#include "Engine/Core/Engine.h"
#include "Engine/Core/GraphicsContext.h"

using namespace AOENGINE;
using namespace PostEffect;

namespace {
constexpr AOENGINE::Color kDefaultColor{ 0.0f, 0.0f, 0.0f, 1.0f };
constexpr float kDefaultScale = 16.0f;
constexpr float kDefaultPower = 0.8f;
constexpr float kMinScale = 0.0f;
constexpr float kMaxScale = 20.0f;
constexpr float kMinPower = 0.0f;
constexpr float kMaxPower = 1.0f;
}

Vignette::~Vignette() {
	settingBuffer_->Destroy();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void Vignette::Init() {
	AOENGINE::GraphicsContext* graphicsCtx = AOENGINE::GraphicsContext::GetInstance();
	settingBuffer_ = graphicsCtx->CreateDxResource(ResourceType::Common);
	settingBuffer_->CreateResource(sizeof(VignetteSetting));
	settingBuffer_->GetResource()->Map(0, nullptr, reinterpret_cast<void**>(&setting_));

	setting_->color = kDefaultColor;
	setting_->scale = kDefaultScale;
	setting_->power = kDefaultPower;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ コマンドを積む
///////////////////////////////////////////////////////////////////////////////////////////////

void Vignette::SetCommand(ID3D12GraphicsCommandList* commandList, AOENGINE::DxResource* pingResource) {
	ApplySaveSettings();
	Engine::SetPipeline(PSOType::ProcessedScene, "PostProcess_Vignette.json");
	Pipeline* pso = Engine::GetLastUsedPipeline();
	UINT index = pso->GetRootSignatureIndex("gTexture");
	commandList->SetGraphicsRootDescriptorTable(index, pingResource->GetSRV().handleGPU);
	index = pso->GetRootSignatureIndex("gSetting");
	commandList->SetGraphicsRootConstantBufferView(index, settingBuffer_->GetResource()->GetGPUVirtualAddress());
	commandList->DrawIndexedInstanced(3, 1, 0, 0, 0);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ チェックボックスの表示
///////////////////////////////////////////////////////////////////////////////////////////////

void Vignette::CheckBox() {
	ImGui::Checkbox("Vignette##Vignette_checkbox", &isEnable_);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 保存項目の適応
///////////////////////////////////////////////////////////////////////////////////////////////

void PostEffect::Vignette::ApplySaveSettings() {
	setting_->color = saveSettings_.color;
	setting_->power = saveSettings_.power;
	setting_->scale = saveSettings_.scale;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 保存
///////////////////////////////////////////////////////////////////////////////////////////////

void PostEffect::Vignette::Save(const std::string& rootField) {
	saveSettings_.isEnable = isEnable_;
	saveSettings_.SetRootField(rootField);
	saveSettings_.Save();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 読み込み
///////////////////////////////////////////////////////////////////////////////////////////////

void PostEffect::Vignette::Load(const std::string& rootField) {
	saveSettings_.SetRootField(rootField);
	saveSettings_.Load();
	isEnable_ = saveSettings_.isEnable;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集処理
///////////////////////////////////////////////////////////////////////////////////////////////

void Vignette::Debug_Gui() {
	if (ImGui::CollapsingHeader("Vignette")) {
		saveSettings_.Debug_Gui();
	}
}

void Vignette::SaveSettings::Debug_Gui() {
	ImGui::ColorEdit4("color", &color.r);
	ImGui::DragFloat("scale", &scale, 0.1f);
	ImGui::DragFloat("power", &power, 0.01f);

	scale = std::clamp(scale, kMinScale, kMaxScale);
	power = std::clamp(power, kMinPower, kMaxPower);
}
