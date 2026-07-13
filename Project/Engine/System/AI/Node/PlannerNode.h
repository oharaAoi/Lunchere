#pragma once
#include <memory>
#include <vector>
#include <unordered_map>
#include "Engine/System/AI/Node/BaseBehaviorNode.h"
#include "Engine/System/AI/BehaviorTree.h"
#include "Engine/System/AI/State/Blackboard.h"
#include "Engine/System/AI/GoalOriented/IOrientedGoal.h"
#include "Engine/System/AI/Other/Condition.h"

namespace AOENGINE {

namespace AI {

/// <summary>
/// GoalまでのNodeたちを格納したNode
/// </summary>
class PlannerNode :
	public BaseBehaviorNode {
public:

	using ActionNode = std::function<std::unique_ptr<BaseBehaviorNode>()>;

public: // constructor

	PlannerNode(const std::unordered_map<std::string, ActionNode>& _creators,
				Blackboard* _worldState,
				const std::vector<std::shared_ptr<IOrientedGoal>>& _goals);
	~PlannerNode() override = default;

public: // public method

	/// <summary>
	/// jsonへ変換
	/// </summary>
	/// <returns></returns>
	json ToJson() override;

	/// <summary>
	/// jsonから保存項目を適応
	/// </summary>
	/// <param name="_jsonData"></param>
	void FromJson(const json& _jsonData) override;

	/// <summary>
	/// 実行
	/// </summary>
	/// <returns></returns>
	BehaviorStatus Execute() override;

	/// <summary>
	/// weight値を算出
	/// </summary>
	/// <returns></returns>
	float EvaluateWeight() override;

	// 実行中のNodeの名前を取得する
	std::string RunNodeName() override;

	// 編集処理
	void Debug_Gui() override;

	/// <summary>
	/// 重みのテーブルに表示する項目
	/// </summary>
	void WeightTableItem() override {};

	/// <summary>
	/// taskの初期化用意
	/// </summary>
	void ResetNode() override;

	/// <summary>
	/// 目標の設定
	/// </summary>
	/// <param name="_orientedName">: 目標の名前</param>
	/// <param name="_treeFileName">: 所有するtreeの名前</param>
	void SetGOBT(const std::string _orientedName, const std::string _treeFileName);

private: // private variable

	BehaviorTree* tree_;
	std::unordered_map<std::string, ActionNode> creators_;
	std::vector<std::shared_ptr<IOrientedGoal>> goalArray_;
	Blackboard* pBlackboard_ = nullptr;

	std::shared_ptr<IOrientedGoal> goal_;

	// 保存用
	std::string orientedName_;
	std::string treeFileName_;

	Condition condition_;

	bool isCreateTree_ = false;
};

}

}

#ifndef AOENGINE_AI_NAMESPACE_ALIAS
#define AOENGINE_AI_NAMESPACE_ALIAS
namespace AI = AOENGINE::AI;
#endif
