#pragma once
#include "Engine/Module/PostEffect/IPostEffect.h"
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Engine/Lib/Math/MathStructures.h"
#include "Engine/Lib/Color.h"

namespace PostEffect {

/// <summary>
/// ディゾルブ
/// </summary>
class Dissolve :
	public IPostEffect {
public:

	/// <summary>
	/// ディゾルブ設定
	/// </summary>
	struct DissolveSetting {
		Math::Matrix4x4 uvTransform;
		AOENGINE::Color color;
		AOENGINE::Color edgeColor;
		float threshold = 0.5f;
	};

	/// <summary>
	/// ディゾルブ保存設定
	/// </summary>
	struct SaveDissolveSetting : public AOENGINE::IJsonConverter {
		bool isEnable = false;
		Math::SRT uvTransform;
		AOENGINE::Color color;
		AOENGINE::Color edgeColor;
		float threshold = 0.5f;

		SaveDissolveSetting() {
			SetGroupName("PostEffect");
			SetName("Dissolve");
		}

		json ToJson(const std::string& id) const override {
			return AOENGINE::JsonBuilder(id)
				.Add("isEnable", isEnable)
				.Add("uvTransform", uvTransform)
				.Add("color", color)
				.Add("edgeColor", edgeColor)
				.Add("threshold", threshold)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			Convert::fromJson(jsonData, "isEnable", isEnable);
			Convert::fromJson(jsonData, "uvTransform", uvTransform);
			Convert::fromJson(jsonData, "color", color);
			Convert::fromJson(jsonData, "edgeColor", edgeColor);
			Convert::fromJson(jsonData, "threshold", threshold);
		}

		void Debug_Gui() override;
	};

public:

	Dissolve() = default;
	~Dissolve() override;

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

	void SetUvTransform(const Math::SRT& srt) { saveSettings_.uvTransform = srt; }

	void SetColor(const AOENGINE::Color& color) { saveSettings_.color = color; }

	void SetEdgeColor(const AOENGINE::Color& color) { saveSettings_.edgeColor = color; }

	void SetThreshold(const float threshold) { saveSettings_.threshold = threshold; }

private:

	AOENGINE::DxResource* settingBuffer_;
	DissolveSetting* setting_;
	SaveDissolveSetting saveSettings_;

};

}