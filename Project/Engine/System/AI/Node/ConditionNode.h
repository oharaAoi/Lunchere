#pragma once
#include "Engine/System/AI/Node/BaseBehaviorNode.h"
#include "Engine/System/AI/State/BlackboardValue.h"

namespace AOENGINE {

namespace AI {

/// <summary>
/// if文の代わりとなるNode
/// </summary>
class ConditionNode :
	public BaseBehaviorNode {
public: // constructor

	ConditionNode();
	~ConditionNode() override = default;

public: // public method

	/// <summary>
	/// jsonへ
	/// </summary>
	/// <returns></returns>
	json ToJson() override;

	/// <summary>
	/// jsonから保存項目を適応
	/// </summary>
	/// <param name="_jsonData"></param>
	void FromJson(const json& _jsonData) override;

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

private: // private method

	/// <summary>
	/// 判断する
	/// </summary>
	bool Compare(const BlackboardValue& lhs,
				 const BlackboardValue& rhs,
				 const std::string& op);

private: // private variable

	static const int32_t kOperatorCount_ = 6;
	const char* conditionOps[kOperatorCount_] = {
	"==", "!=", ">", "<", ">=", "<="
	};

	// 各キーのインデックス
	int32_t opIndex_ = 0;
	int32_t leftKeyIndex_ = 0;
	int32_t rightKeyIndex_ = 0;

	int32_t radioButtonIndex_ = 0;
	uint32_t preIndex_ = 0;

	// キー
	std::string leftKey_ = "";
	std::string rightKey_ = "";

	// 評価に使用する値
	float freeValue_ = 0;
	BlackboardValue value_;

};

}

}

#ifndef AOENGINE_AI_NAMESPACE_ALIAS
#define AOENGINE_AI_NAMESPACE_ALIAS
namespace AI = AOENGINE::AI;
#endif
