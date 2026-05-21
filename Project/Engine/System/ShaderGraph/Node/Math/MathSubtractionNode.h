#pragma once
#include "Engine/System/ShaderGraph/Node/BaseShaderGraphNode.h"

namespace AOENGINE {

/// <summary>
/// 減算ノードクラス
/// </summary>
class MathSubtractionNode :
	public BaseShaderGraphNode {
public:	// コンストラクタ

	MathSubtractionNode() {};
	~MathSubtractionNode() override = default;

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
	void updateGui() override {};

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

	float a_;
	float b_;
	float value_;
};

}