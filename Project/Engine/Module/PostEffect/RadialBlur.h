#pragma once
#include "Engine/Lib/Math/Vector2.h"
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Engine/Module/PostEffect/IPostEffect.h"
#include "Engine/Module/Components/Animation/VectorTween.h"

namespace PostEffect {

/// <summary>
/// 放射ブラー
/// </summary>
class RadialBlur :
	public IPostEffect {
public:

	/// <summary>
	/// ラジアルブラー設定
	/// </summary>
	struct BlurSetting {
		Math::Vector2 blurCenter = { 0.5f, 0.5f };	// 放射ブラーの中心（通常はfloat2(0.5, 0.5)）
		float blurStrength = 0.00f;			// ブラーの強度（例: 0.02）
		float blurStart = 0.2f;				// ブラーの開始距離
		uint32_t sampleCount = 16;				// サンプル数（例: 16）
	};

	/// <summary>
	/// ラジアルブラー保存設定
	/// </summary>
	struct SaveSetting : public AOENGINE::IJsonConverter {
		bool isEnable = false;
		uint32_t sampleCount;

		SaveSetting() {
			SetGroupName("PostEffect");
			SetName("RadialBlur");
		}

		json ToJson(const std::string& id) const override {
			return AOENGINE::JsonBuilder(id)
				.Add("isEnable", isEnable)
				.Add("sampleCount", sampleCount)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			Convert::fromJson(jsonData, "isEnable", isEnable);
			Convert::fromJson(jsonData, "sampleCount", sampleCount);
		}

		void Debug_Gui() override {};
	};

public:

	RadialBlur() = default;
	~RadialBlur() override;

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
	/// 開始処理
	/// </summary>
	/// <param name="_strength">: 強さ</param>
	/// <param name="_startTime">: 時間</param>
	/// <param name="_continuation">: 継続するか</param>
	void Start(float _strength, float _startTime, bool _continuation);

	/// <summary>
	/// 徐々に止める
	/// </summary>
	/// <param name="time"></param>
	void SlowDown(float time);

	/// <summary>
	/// 止める
	/// </summary>
	void Stop();

public:

	void SetStrength(float strength) { setting_->blurStrength = strength; }

private:

	AOENGINE::DxResource* blurSettingBuffer_;
	BlurSetting* setting_;
	SaveSetting saveSettings_;

	bool run_ = false;
	bool continuation_ = false; // 継続するか
	AOENGINE::VectorTween<float> blurStrengthTween_;
};

}