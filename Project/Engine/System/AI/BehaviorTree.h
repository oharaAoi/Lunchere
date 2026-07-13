#pragma once
#include <list>
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>
#include "Engine/System/AI/BehaviorTreeEditor.h"
#include "Engine/System/AI/Node/BaseBehaviorNode.h"
#include "Engine/System/AI/Node/BehaviorRootNode.h"
#include "Engine/System/AI/Node/BaseTaskNode.h"
#include "Engine/System/AI/State/Blackboard.h"
#include "Engine/System/AI/GoalOriented/IOrientedGoal.h"
#include "Engine/System/Manager/ImGuiManager.h"
#include "Engine/Module/Components/GameObject/BaseEntity.h"
#include "Engine/Utilities/BehaviorTreeLogger.h"
#include "Engine/System/Manager/ImGuiManager.h"

namespace AOENGINE {

namespace AI {

/// <summary>
/// Objectを制御するためのクラス
/// </summary>
class BehaviorTree {
public: // data

	using ActionNode = std::function<std::unique_ptr<BaseBehaviorNode>()>;

public: // constructor

	BehaviorTree() = default;
	~BehaviorTree();

public: // public method

	// 初期化処理
	void Init();

	// 実行処理
	bool Run();

	// 編集処理
	void Edit();

	// 選択したものを編集
	void EditSelect();

	// タスクの追加
	void Register(const std::string& name, ActionNode creator) {
		creators_[name] = std::move(creator);
	}

	/// <summary>
	/// Treeの作成
	/// </summary>
	/// <param name="nodeName"></param>
	void CreateTree(const std::string& fileDirectory, const std::string& fileName);

	/// <summary>
	/// treeの所有者の設定
	/// </summary>
	/// <param name="_target"></param>
	void SetTarget(AOENGINE::BaseEntity* _target) { pTarget_ = _target; }

	/// <summary>
	/// 実行設定の処理
	/// </summary>
	/// <param name="_isExecute"></param>
	void SetExecute(bool _isExecute) { isExecute_ = _isExecute; }

	/// <summary>
	/// 実行できるタスクの設定
	/// </summary>
	/// <param name="_canTaskMap"></param>
	void SetCanTaskMap(const std::unordered_map<std::string, ActionNode>& _creators);

	/// <summary>
	/// 目標の設定
	/// </summary>
	/// <param name="_goal"></param>
	void AddGoal(std::shared_ptr<IOrientedGoal> _goal);

	/// <summary>
	/// 現在の状態を表示する
	/// </summary>
	/// <param name="ownerWorldPos">: Treeの所有者のワールド座標</param>
	/// <param name="_aggressionScore">: 積極性の値</param>
	void DisplayState(const ImVec2& _pos, float _aggressionScore, const ImVec2& _offset = {-30.0f, -30.0f});

public: // accessor

	void SetName(const std::string& _name) { name_ = _name; }
	const std::string& GetName() const { return name_; }

	void SetBlackboard(Blackboard* _blackboard) { blackboard_ = _blackboard; }

	BaseBehaviorNode* GetRootNode() const { return root_; }

	void SetIsOpenEditor(bool isOpen) { isOpenEditor_ = isOpen; }
	bool GetIsOpenEditor() const { return isOpenEditor_; }

	void SetIsDelete(bool isDelete) { isDelete_ = isDelete; }
	bool GetIsDelete() const { return isDelete_; }

private: // private variable

	std::string name_ = "Behavior Tree";

	Blackboard* blackboard_ = nullptr;

	// treeの所有者のポインタ
	AOENGINE::BaseEntity* pTarget_ = nullptr;
	// 接続のidをまとめたコンテナ
	std::vector<Link> links_;
	// nodeのリスト
	std::list<std:: unique_ptr<BaseBehaviorNode>> nodeList_;
	// 最上位Node
	BaseBehaviorNode* root_ = nullptr;
	BehaviorStatus rootState_;
	// 行えるTaskをまとめた物
	std::unordered_map<std::string, ActionNode> creators_;

	std::vector<std::shared_ptr<IOrientedGoal>> goalArray_;

	BehaviorTreeEditor editor_;

	// フラグ
	bool isExecute_ = false;
	bool isOpenEditor_ = false;
	bool isDelete_ = false;

	// ----------------------
	// ↓ loggerポインタ
	// ----------------------

	std::unique_ptr<AOENGINE::BehaviorTreeLogger> logger_ = nullptr;;

	std::string fileDirectory_;
	std::string fileName_;

};
}

}

#ifndef AOENGINE_AI_NAMESPACE_ALIAS
#define AOENGINE_AI_NAMESPACE_ALIAS
namespace AI = AOENGINE::AI;
#endif


template<typename ActionT, typename Target>
std::unique_ptr<AI::BaseBehaviorNode> CreateTask(Target* target, const std::string& nodeName) {
	auto result = std::make_unique<ActionT>();
	result->SetTarget(target);
	result->SetNodeName(nodeName);
	return (result);
}