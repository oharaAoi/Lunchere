#pragma once
#include "Engine/System/ShaderGraph/Node/BaseShaderGraphNode.h"
#include "Engine/DirectX/Resource/DxResource.h"
#include "Engine/Core/GraphicsContext.h"

namespace AOENGINE {

/// <summary>
/// 歪みを行なうNode
/// </summary>
class DistortionNode :
	public BaseShaderGraphNode {
public:	// データ構造体

	/// <summary>
	/// ディストーション用パラメータ
	/// </summary>
	struct DistortionParam {
		Math::Vector2 tiling = CMath::Vector2::UNIT;
		Math::Vector2 scroll = CMath::Vector2::UNIT;;
		float strength = 1.0f;
		float time = 0;
	};

public:	// コンストラクタ

	DistortionNode() = default;
	~DistortionNode() override;

public:

	/// <summary>
	/// 初期化関数
	/// </summary>
	void Init() override;

	/// <summary>
	/// 更新関数
	/// </summary>
	void customUpdate() override;

	/// <summary>
	/// guiの更新
	/// </summary>
	void updateGui() override;

	/// <summary>
	/// Node描画
	/// </summary>
	void draw() override;

	/// <summary>
	/// json形式にする
	/// </summary>
	/// <returns></returns>
	nlohmann::json toJson() override;

	/// <summary>
	/// json形式から情報を設定する
	/// </summary>
	/// <param name="_json"></param>
	void fromJson(const nlohmann::json& _json) override;

private:

	/// <summary>
	/// Viewの作成
	/// </summary>
	void CreateView();

	/// <summary>
	/// Csを実行する
	/// </summary>
	void ExecuteCommand();

private:

	AOENGINE::GraphicsContext* ctx_;
	ID3D12GraphicsCommandList* cmdList_;

	// input情報
	AOENGINE::DxResource* inputBaseResource_ = nullptr;
	AOENGINE::DxResource* inputNoiseResource_ = nullptr;

	// output情報
	AOENGINE::DxResource* outputResource_ = nullptr;

	// distortion情報
	ComPtr<ID3D12Resource> buffer_;
	DistortionParam* param_;

};

}