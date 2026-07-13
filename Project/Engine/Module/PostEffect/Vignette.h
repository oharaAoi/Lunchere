#pragma once
#include "Engine/Lib/Color.h"
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Engine/Module/PostEffect/IPostEffect.h"

namespace AOENGINE {

namespace PostEffect {

/// <summary>
/// ヴィネット
/// </summary>
class Vignette :
	public IPostEffect {
public:	// 構造体

	/// <summary>
	/// ビネット設定
	/// </summary>
	struct VignetteSetting {
		AOENGINE::Color color;
		float scale = 16.0f;
		float power = 0.8f;
	};

	/// <summary>
	/// ビネット保存設定
	/// </summary>
	struct SaveSettings : public AOENGINE::IJsonConverter {
		bool isEnable = false;
		AOENGINE::Color color;
		float scale = 16.0f;
		float power = 0.8f;

		SaveSettings() {
			SetGroupName("PostEffect");
			SetName("Vignette");
		}

		json ToJson(const std::string& id) const override {
			return AOENGINE::JsonBuilder(id)
				.Add("isEnable", isEnable)
				.Add("color", color)
				.Add("scale", scale)
				.Add("power", power)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			Convert::fromJson(jsonData, "isEnable", isEnable);
			Convert::fromJson(jsonData, "color", color);
			Convert::fromJson(jsonData, "scale", scale);
			Convert::fromJson(jsonData, "power", power);
		}

		void Debug_Gui() override;
	};

public:

	Vignette() = default;
	~Vignette() override;

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

public:

	void SetPower(float _power) { saveSettings_.power = _power; }

	void SetScale(float _scale) { saveSettings_.scale = _scale; }

	void SetColor(const AOENGINE::Color& _color) { saveSettings_.color = _color; }

private:

	AOENGINE::DxResource* settingBuffer_;
	VignetteSetting* setting_;
	SaveSettings saveSettings_;
};
}

}

#ifndef AOENGINE_POSTEFFECT_NAMESPACE_ALIAS
#define AOENGINE_POSTEFFECT_NAMESPACE_ALIAS
namespace PostEffect = AOENGINE::PostEffect;
#endif
