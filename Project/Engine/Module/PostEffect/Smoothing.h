#pragma once
#include "Engine/Module/PostEffect/IPostEffect.h"
#include "Engine/Lib/Json/IJsonConverter.h"

namespace AOENGINE {

namespace PostEffect {

/// <summary>
/// Smoothing処理
/// </summary>
class Smoothing :
	public IPostEffect {
public:

	/// <summary>
	/// 平滑化設定
	/// </summary>
	struct Setting {
		uint32_t size;
	};

	/// <summary>
	/// 平滑化保存設定
	/// </summary>
	struct SaveSettings : public AOENGINE::IJsonConverter {
		bool isEnable = false;
		uint32_t size = 1;

		SaveSettings() {
			SetGroupName("PostEffect");
			SetName("Smoothing");
		}

		json ToJson(const std::string& id) const override {
			return AOENGINE::JsonBuilder(id)
				.Add("isEnable", isEnable)
				.Add("size", size)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			Convert::fromJson(jsonData, "isEnable", isEnable);
			Convert::fromJson(jsonData, "size", size);
		}

		void Debug_Gui() override;
	};

public:

	Smoothing() = default;
	~Smoothing() override;

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

public: // accessor

	void SetSize(uint32_t size) { saveSettings_.size = size; }

private:

	AOENGINE::DxResource* settingBuffer_;
	Setting* setting_;
	SaveSettings saveSettings_;

};

}

}

#ifndef AOENGINE_POSTEFFECT_NAMESPACE_ALIAS
#define AOENGINE_POSTEFFECT_NAMESPACE_ALIAS
namespace PostEffect = AOENGINE::PostEffect;
#endif
