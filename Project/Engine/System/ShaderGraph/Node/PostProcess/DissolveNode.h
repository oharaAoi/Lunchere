#pragma once
#include "Engine/System/ShaderGraph/Node/BaseShaderGraphNode.h"
#include "Engine/DirectX/Resource/DxResource.h"
#include "Engine/Core/GraphicsContext.h"

namespace AOENGINE {

/// <summary>
/// Dissolve処理を行なうNode
/// </summary>
class DissolveNode :
	public BaseShaderGraphNode {
public:	// データ構造体

	/// <summary>
	/// ディゾルブ用パラメータ
	/// </summary>
	struct DissolveParams {
		AOENGINE::Color dissolveColor; // エッジの色
		float threshold;	// しきい値
		float edgeWidth;	// エッジ幅
	};

public:	// コンストラクタ

	DissolveNode() = default;
	~DissolveNode() override;

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
	DissolveParams* param_;
	float threshold_;
};

}