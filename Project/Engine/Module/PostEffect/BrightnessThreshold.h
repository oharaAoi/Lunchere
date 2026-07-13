#pragma once
#include "Engine/Module/PostEffect/IPostEffect.h"
#include "Engine/Lib/Json/IJsonConverter.h"

namespace AOENGINE {

namespace PostEffect {

/// <summary>
/// 閾値以上の輝度を抽出
/// </summary>
class BrightnessThreshold :
	public IPostEffect {
public:	// 構造体

	/// <summary>
	/// 輝度抽出設定
	/// </summary>
	struct BloomSettings {
		float threshold = 1.0f;
	};

	/// <summary>
	/// 輝度抽出保存設定
	/// </summary>
	struct SaveBloomSettings : public AOENGINE::IJsonConverter {
		float threshold = 0.5f;

		SaveBloomSettings() {
			SetGroupName("PostEffect");
			SetName("BrightnessThreshold");
		}

		json ToJson(const std::string& id) const override {
			return AOENGINE::JsonBuilder(id)
				.Add("threshold", threshold)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			Convert::fromJson(jsonData, "threshold", threshold);
		}

		void Debug_Gui() override;
	};

public:

	BrightnessThreshold() = default;
	~BrightnessThreshold() override;

public:

	/// <summary>
	/// 初期化
	/// </summary>
	void Init() override;

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

private:

	AOENGINE::DxResource* bloomBuffer_;
	BloomSettings* bloomSetting_;
	SaveBloomSettings saveBloomSettings_;

};

}

}

#ifndef AOENGINE_POSTEFFECT_NAMESPACE_ALIAS
#define AOENGINE_POSTEFFECT_NAMESPACE_ALIAS
namespace PostEffect = AOENGINE::PostEffect;
#endif
