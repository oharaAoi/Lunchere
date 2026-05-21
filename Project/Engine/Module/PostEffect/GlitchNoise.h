#pragma once
#include "Engine/Lib/Math/Vector2.h"
#include "Engine/Module/PostEffect/IPostEffect.h"
#include "Engine/Lib/Json/IJsonConverter.h"

namespace PostEffect {

/// <summary>
/// グリッチエフェクト
/// </summary>
class GlitchNoise :
	public IPostEffect {
public:

	/// <summary>
	/// グリッチノイズ設定
	/// </summary>
	struct GlitchSetting {
		Math::Vector2 texelSize;	// texelのサイズ
		float time;			// 時間
		float strength;
		uint32_t frameIndex;
	};

	/// <summary>
	/// グリッチノイズ保存設定
	/// </summary>
	struct SaveSettings : public AOENGINE::IJsonConverter {
		bool isEnable = false;
		Math::Vector2 texelSize = Math::Vector2(1, 1);
		
		SaveSettings() {
			SetGroupName("PostEffect");
			SetName("GlitchNoise");
		}

		json ToJson(const std::string& id) const override {
			return AOENGINE::JsonBuilder(id)
				.Add("isEnable", isEnable)
				.Add("texelSize", texelSize)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			Convert::fromJson(jsonData, "isEnable", isEnable);
			Convert::fromJson(jsonData, "texelSize", texelSize);
		}

		void Debug_Gui() override;
	};

public:

	GlitchNoise() = default;
	~GlitchNoise() override;

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

	/// <summary>
	/// Noiseを実行させる
	/// </summary>
	/// <param name="startStrength">: 開始の強さ</param>
	/// <param name="time">: ノイズの時間</param>
	void StartNoise(float startStrength, float time);

public: // accessor

	void SetTexelSize(const Math::Vector2& texelSize) { saveSettings_.texelSize = texelSize; }

private:

	AOENGINE::DxResource* glitchBuffer_;
	GlitchSetting* setting_;
	SaveSettings saveSettings_;

	float noiseTime_;
};

}