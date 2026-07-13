#pragma once
#include <unordered_map>
#include "Engine/System/AI/Node/BaseBehaviorNode.h"

namespace AOENGINE {

namespace AI {

/// <summary>
/// 重みの計算を行ってから選択をするセレクタ
/// </summary>
class WeightSelectorNode :
	public BaseBehaviorNode {
public: // constructor

	WeightSelectorNode();
	~WeightSelectorNode() override = default;

public: // public method

	// jsonへ
	json ToJson() override;

	// 実行
	BehaviorStatus Execute() override;

	// ウェイト値の算出
	float EvaluateWeight() override { return 0; }

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

private: // private method

	/// <summary>
	/// 評価値を表示する
	/// </summary>
	void PriorityDisplay();

	/// <summary>
	/// 評価値を外部に出す
	/// </summary>
	void PriorityOutput();

private: // private variable

	std::unordered_map<uint32_t, float> weightMap_;
	bool isReset_ = false;

};

}

}

#ifndef AOENGINE_AI_NAMESPACE_ALIAS
#define AOENGINE_AI_NAMESPACE_ALIAS
namespace AI = AOENGINE::AI;
#endif
