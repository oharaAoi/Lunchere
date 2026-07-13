#pragma once
#include <list>
#include <memory>
#include <functional>
#include <unordered_map>
#include <string>
#include "Engine/System/AI/Node/BaseBehaviorNode.h"
#include "Engine/System/AI/GoalOriented/IOrientedGoal.h"

namespace AOENGINE {

namespace AI{

/// <summary>
/// Nodeを作成するクラス
/// </summary>

namespace BehaviorTreeNodeFactory {

	using ActionNode = std::function<std::unique_ptr<BaseBehaviorNode>()>;


	/// <summary>
	/// Nodeを作成する
	/// </summary>
	/// <param name="nodeType"></param>
	/// <param name="crateTaskName"></param>
	/// <param name="_nodeList"></param>
	/// <param name="_worldState"></param>
	/// <param name="_canTaskMap"></param>
	/// <param name="_goalArray"></param>
	void CreateNode(int nodeType,
						   const std::string& crateTaskName,
						   std::list<std::unique_ptr<BaseBehaviorNode>>& _nodeList,
						   Blackboard* _worldState,
						   const std::unordered_map<std::string, ActionNode>& _creators,
						   const std::vector<std::shared_ptr<IOrientedGoal>>& _goalArray, const ImVec2& _mousePos);

	/// <summary>
	/// Jsonファイルから読み込んでNodeを作成する
	/// </summary>
	/// <param name="_json"></param>
	/// <param name="_nodeList"></param>
	/// <param name="_link"></param>
	/// <param name="_worldState"></param>
	/// <param name="_canTaskMap"></param>
	/// <param name="_goalArray"></param>
	/// <returns></returns>
	BaseBehaviorNode* CreateNodeFromJson(const json& _json,
															 std::list<std::unique_ptr<BaseBehaviorNode>>& _nodeList,
															 std::vector<Link>& _link,
															 Blackboard* _worldState,
															 const std::unordered_map<std::string, ActionNode>& _creators,
															 const std::vector<std::shared_ptr<IOrientedGoal>>& _goalArray);

	/// <summary>
	/// Treeを作成する
	/// </summary>
	/// <param name="nodeName"></param>
	/// <param name="_nodeList"></param>
	/// <param name="_link"></param>
	/// <param name="_root"></param>
	/// <param name="_worldState"></param>
	/// <param name="_canTaskMap"></param>
	/// <param name="_goalArray"></param>
	void CreateTree(const json& _json,
						   std::list<std::unique_ptr<BaseBehaviorNode>>& _nodeList,
						   std::vector<Link>& _link,
						   BaseBehaviorNode* _root,
						   Blackboard* _worldState,
						   const std::unordered_map<std::string, ActionNode>& _creators,
						   const std::vector<std::shared_ptr<IOrientedGoal>>& _goalArray);

	/// <summary>
	/// ActionNodeの作成
	/// </summary>
	/// <param name="_creators"></param>
	/// <param name="_name"></param>
	/// <returns></returns>
	std::unique_ptr<BaseBehaviorNode> CreateActionNode(const std::unordered_map<std::string, ActionNode>& _creators, const std::string& _name);

};

}

}

#ifndef AOENGINE_AI_NAMESPACE_ALIAS
#define AOENGINE_AI_NAMESPACE_ALIAS
namespace AI = AOENGINE::AI;
#endif