#pragma once
#include "Engine/Lib/Color.h"
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Engine/Module/PostEffect/IPostEffect.h"

namespace PostEffect {

/// <summary>
/// 画面全体を灰色にする
/// </summary>
class Grayscale :
	public IPostEffect {
public:

	/// <summary>
	/// グレースケール設定
	/// </summary>
	struct Setting {
		AOENGINE::Color color;
	};

	/// <summary>
	/// グレースケール保存設定
	/// </summary>
	struct SaveSettings : public AOENGINE::IJsonConverter {
		bool isEnable = false;
		AOENGINE::Color color = Colors::Linear::black;

		SaveSettings() {
			SetGroupName("PostEffect");
			SetName("Grayscale");
		}

		json ToJson(const std::string& id) const override {
			return AOENGINE::JsonBuilder(id)
				.Add("isEnable", isEnable)
				.Add("color", color)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			Convert::fromJson(jsonData, "isEnable", isEnable);
			Convert::fromJson(jsonData, "color", color);
		}

		void Debug_Gui() override;
	};

public:

	Grayscale() = default;
	~Grayscale() override;

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

	void SetColor(const AOENGINE::Color& _color) { saveSettings_.color = _color; }

private:

	AOENGINE::DxResource* settingBuffer_;
	Setting* setting_;
	SaveSettings saveSettings_;
};
}
