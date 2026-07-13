#pragma once
#include "Engine/Module/PostEffect/BrightnessThreshold.h"
#include "Engine/Module/PostEffect/GaussianBlurWidth.h"
#include "Engine/Module/PostEffect/GaussianBlurHeight.h"
#include "Engine/Module/PostEffect/PingPongBuffer.h"
#include "Engine/Lib/Json/IJsonConverter.h"

namespace AOENGINE {

namespace PostEffect {

/// <summary>
/// Bloomクラス
/// </summary>
class Bloom :
	public IPostEffect {
public:	// 構造体

	/// <summary>
	/// Bloom設定
	/// </summary>
	struct BloomSettings {
		float bloomIntensity = 0.5f;
	};

	/// <summary>
	/// Bloom保存設定
	/// </summary>
	struct SaveBloomSettings : public AOENGINE::IJsonConverter {
		bool isEnable = false;
		float bloomIntensity = 0.5f;

		SaveBloomSettings() {
			SetGroupName("PostEffect");
			SetName("Bloom");
		}

		json ToJson(const std::string& id) const override {
			return AOENGINE::JsonBuilder(id)
				.Add("isEnable", isEnable)
				.Add("bloomIntensity", bloomIntensity)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			Convert::fromJson(jsonData, "isEnable", isEnable);
			Convert::fromJson(jsonData, "bloomIntensity", bloomIntensity);
		}

		void Debug_Gui() override;
	};

public:

	Bloom() = default;
	~Bloom() override;

public:

	/// <summary>
	/// 初期化
	/// </summary>
	void Init() override;

	/// <summary>
	/// 特殊初期化
	/// </summary>
	/// <param name="_owner"></param>
	void PostInit(AOENGINE::PostProcess* _owner) override;

	/// <summary>
	/// コマンドを積む
	/// </summary>
	/// <param name="commandList"></param>
	/// <param name="pingResource"></param>
	void SetCommand(ID3D12GraphicsCommandList* commandList, AOENGINE::DxResource* pingResource) override;

	/// <summary>
	/// チェックボックスの表示
	/// </summary>
	void CheckBox() override;

	/// <summary>
	/// 保存項目の適応
	/// </summary>
	void ApplySaveSettings() override;

	/// <summary>
	/// 保存
	/// </summary>
	/// <param name="rootField">: PostEffectの項目の一つ上のフォルダ名</param>
	void Save(const std::string& rootField) override;

	/// <summary>
	/// 読み込み
	/// </summary>
	/// <param name="rootField">: PostEffectの項目の一つ上のフォルダ名</param>
	void Load(const std::string& rootField) override;

	/// <summary>
	/// 編集処理
	/// </summary>
	void Debug_Gui() override;


public:

	/// <summary>
	/// 最終的に書き込むresourceを取得
	/// </summary>
	/// <param name="_resource"></param>
	void SetPongResource(AOENGINE::PingPongBuffer* _resource) { postProcessResource_ = _resource; }

	/// <summary>
	/// 深度のハンドルを取得
	/// </summary>
	/// <param name="handle"></param>
	void SetDepthHandle(D3D12_CPU_DESCRIPTOR_HANDLE handle) { depthHandle_ = handle; }

	/// <summary>
	/// 輝度の設定
	/// </summary>
	/// <param name="intensity"></param>
	void SetBloomIntensity(float intensity) { saveSettings_.bloomIntensity = intensity; }

private:

	std::unique_ptr<AOENGINE::PingPongBuffer> pingPongBuff_;

	std::unique_ptr<PostEffect::BrightnessThreshold> brightnessBuffer_;
	std::unique_ptr<PostEffect::GaussianBlurWidth> blurWidthBuffer_;
	std::unique_ptr<PostEffect::GaussianBlurHeight> blurHeightBuffer_;

	AOENGINE::DxResource* settingBuffer_;
	BloomSettings* setting_;
	SaveBloomSettings saveSettings_;

	AOENGINE::PingPongBuffer* postProcessResource_;
	AOENGINE::DxResource* sceneBuffer_;


	ID3D12Device* device_ = nullptr;
	AOENGINE::DescriptorHeap* dxHeap_ = nullptr;

	D3D12_CPU_DESCRIPTOR_HANDLE depthHandle_;
};

}

}

#ifndef AOENGINE_POSTEFFECT_NAMESPACE_ALIAS
#define AOENGINE_POSTEFFECT_NAMESPACE_ALIAS
namespace PostEffect = AOENGINE::PostEffect;
#endif
