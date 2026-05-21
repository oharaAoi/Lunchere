#pragma once
#include "Engine/Module/PostEffect/IPostEffect.h"
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Engine/Lib/Math/Vector2.h"

namespace PostEffect {

/// <summary>
/// 縦幅のブラー
/// </summary>
class GaussianBlurHeight :
	public IPostEffect {
public:	// 構造体

	/// <summary>
	/// 縦方向ガウスブラー設定
	/// </summary>
	struct BlurSettings {
		Math::Vector2 texelSize;
	};

	/// <summary>
	/// 縦方向ガウスブラー保存設定
	/// </summary>
	struct SaveSettings : public AOENGINE::IJsonConverter {
		uint32_t sampleCount = 1;

		SaveSettings() {
			SetGroupName("PostEffect");
			SetName("GaussianBlurHeight");
		}

		json ToJson(const std::string& id) const override {
			return AOENGINE::JsonBuilder(id)
				.Add("sampleCount", sampleCount)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			Convert::fromJson(jsonData, "sampleCount", sampleCount);
		}

		void Debug_Gui() override;
	};

public:

	GaussianBlurHeight() = default;
	~GaussianBlurHeight() override;

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

private:

	AOENGINE::DxResource* blurBuffer_;
	BlurSettings* blurSetting_;
	SaveSettings saveSettings_;
};

}