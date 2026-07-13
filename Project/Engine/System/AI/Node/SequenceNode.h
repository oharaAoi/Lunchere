#pragma once
#include "Engine/System/AI/Node/BaseBehaviorNode.h"

namespace AOENGINE {

namespace AI {

/// <summary>
/// 一番左の子が成功したら次の子へと遷移するNode
/// </summary>
class SequenceNode :
	public BaseBehaviorNode {
public: // constructor

	SequenceNode();
	~SequenceNode() override = default;

public: // public method

	// jsonへ
	json ToJson() override;

	// 実行
	BehaviorStatus Execute() override;

	// ウェイト値の算出
	float EvaluateWeight() override;

	// 編集処理
	void Debug_Gui() override;

	// 実行中のNodeの名前を取得する
	std::string RunNodeName() override;

	/// <summary>
	/// 重みのテーブルに表示する項目
	/// </summary>
	void WeightTableItem() override {};

	/// <summary>
	/// taskの初期化用意
	/// </summary>
	void ResetNode() override;
};

}

}

#ifndef AOENGINE_AI_NAMESPACE_ALIAS
#define AOENGINE_AI_NAMESPACE_ALIAS
namespace AI = AOENGINE::AI;
#endif
