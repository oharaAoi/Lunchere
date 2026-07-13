#pragma once
#include "Engine/Module/PostEffect/IPostEffect.h"
#include "Engine/Lib/Math/Matrix4x4.h"
#include "Engine/Lib/Json/IJsonConverter.h"

namespace AOENGINE {

namespace PostEffect {

/// <summary>
/// 深度ベースアウトライン
/// </summary>
class DepthBasedOutline :
	public IPostEffect {
public:

	/// <summary>
	/// 深度ベースアウトライン設定
	/// </summary>
	struct Setting {
		Math::Matrix4x4 projectionInverse;
		float edgeGain;
	};

	/// <summary>
	/// 深度ベースアウトライン保存設定
	/// </summary>
	struct SaveSetting : public AOENGINE::IJsonConverter {
		bool isEnable = false;
		float edgeGain;

		SaveSetting() {
			SetGroupName("PostEffect");
			SetName("DepthBasedOutline");
		}

		json ToJson(const std::string& id) const override {
			return AOENGINE::JsonBuilder(id)
				.Add("isEnable", isEnable)
				.Add("edgeGain", edgeGain)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			Convert::fromJson(jsonData, "isEnable", isEnable);
			Convert::fromJson(jsonData, "edgeGain", edgeGain);
		}

		void Debug_Gui() override;
	};

public:

	DepthBasedOutline() = default;
	~DepthBasedOutline() override;

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

	void SetEdgeGain(float edge) { saveSettings_.edgeGain = edge; }

private:

	AOENGINE::DxResource* settingBuffer_;
	Setting* setting_;
	SaveSetting saveSettings_;

	ID3D12Resource* depthResource_;
	DescriptorHandles depthHandle_;
};
}

}

#ifndef AOENGINE_POSTEFFECT_NAMESPACE_ALIAS
#define AOENGINE_POSTEFFECT_NAMESPACE_ALIAS
namespace PostEffect = AOENGINE::PostEffect;
#endif
