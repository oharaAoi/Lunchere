#pragma once
#include "Engine/System/ShaderGraph/Node/BaseShaderGraphNode.h"
#include "Engine/Module/Components/Animation/VectorTween.h"

namespace AOENGINE {

/// <summary>
/// ループ値ノードクラス
/// </summary>
class LoopValueNode :
	public BaseShaderGraphNode {
public:	// コンストラクタ

	LoopValueNode();
	~LoopValueNode() override = default;

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

	VectorTween<float> timer_;

	float output_ = 0;

	float start_ = 0;
	float end_ = 0;
	float duration_ = 0;
	int easeIndex_ = 0;
	int loopType_ = 0;

};

}