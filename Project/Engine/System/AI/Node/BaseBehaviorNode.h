#pragma once
#include <string>
#include <vector>
#include <memory>
#include "Engine/System/Manager/ImGuiManager.h"
#include "Engine/Module/Components/Attribute/AttributeGui.h"
#include "Engine/System/AI/State/Blackboard.h"
#include "Engine/Utilities/BehaviorTreeLogger.h"
#include "Engine/Utilities/Timer.h"
#include "Engine/Lib/Math/Vector2.h"
#include "Engine/Lib/Json/IJsonConverter.h"
#include "Engine/Lib/Node/NodeItems.h"


enum BehaviorStatus {
	Inactive,	// 非アクティブ
	Success,	// 成功
	Failure,	// 失敗
	Running		// 実行中
};

enum class NodeType {
	Root,
	Sequencer,
	Selector,
	WeightSelector,
	Task,
	Planner,
	PlannerSelector,
	Condition,
	Parallel
};

namespace AOENGINE {

namespace AI {

/// <summary>
/// 各Nodeで共通となる基礎クラス
/// </summary>
class BaseBehaviorNode :
	public AOENGINE::AttributeGui {
public: // constructor

	BaseBehaviorNode();
	virtual ~BaseBehaviorNode() = default;
	
public: // public method

	// 初期化処理
	void Init();

	// 更新処理
	void Update();

	// nodeの描画
	void DrawNode();

	// indexをリセットする
	void ResetIndex();

	// 子の追加
	void AddChild(BaseBehaviorNode* child);

	// 子の削除
	void DeleteChild(BaseBehaviorNode* child);

	// 子を削除する
	void ClearChild();

	// 保存項目を適応する
	virtual void FromJson(const json& _jsonData);

	// 編集関数
	virtual void Debug_Gui() override {};

	// 現在選択されているNodeかどうか
	bool IsSelectNode();

	// Nodeの名前をつなぐ
	std::string NodeNameCombination();

public:

	// 実行関数
	virtual BehaviorStatus Execute() = 0;

	// 評価値の算出を行う
	virtual float EvaluateWeight() = 0;

	// 実行中のNodeの名前を取得する
	virtual std::string RunNodeName() = 0;

	// json形式への変換
	virtual json ToJson() = 0;

	// 重みのテーブルに表示する項目
	virtual void WeightTableItem() = 0;

	// Nodeのリセット
	virtual void ResetNode() = 0;

	// 現在実行中のNodeの名前を返す
	std::string BaseRunNodeName();

private: // private method

	/// <summary>
	/// NodeにLineを描画
	/// </summary>
	/// <param name="_texPos"></param>
	void DrawImGuiLine(const ImVec2& _texPos);

public: // accessor

	ax::NodeEditor::NodeId GetId() { return node_.id; }

	static uint32_t GetNextId();

	const Pin& GetInput() const { return node_.inputId; }
	const Pin& GetOutput() const { return node_.outputId; }

	void SetNodeName(const std::string& _name) {
		node_.name = _name;
		name_ = _name;
	}
	const std::string& GetNodeName() { return node_.name; }

	void EditNodeName();

	void SetNodeType(NodeType _type) { type_ = _type; }
	NodeType GetNodeType() const { return type_; }

	void SetState(BehaviorStatus state) { state_ = state; }
	BehaviorStatus GetState() const { return state_; }

	bool GetIsDelete() const { return isDelete_; }

	void SetPos(const Math::Vector2& _pos) {
		setNodePos_ = false;
		pos_ = _pos;
	}
	Math::Vector2 GetPos() { return pos_; }

	const std::vector<BaseBehaviorNode*>& GetChildren() const { return children_; }

	void SetBlackboard(Blackboard* _worldState) { blackboard_ = _worldState; }

	const std::string GetCurrentRunNodeName() const { return currentRunNodeName_; }

protected: // protected variable

	static uint32_t nextSerialNumber_;	// 次のユニークid

	Node node_;				// node本体
	NodeType type_;			// nodeのタイプ
	BehaviorStatus state_;	// nodeの状態

	std::vector<BaseBehaviorNode*> children_;	// 子ども
	uint32_t currentIndex_;					// 現在参照している子のindex

	bool isLeafNode_ = false;				// リーフノードかどうか

	Math::Vector2 pos_;				// Nodeの座標
	bool setNodePos_;			// Node座標の設定を行ったかどうか

	Blackboard* blackboard_;

	AOENGINE::Timer coolTimer_;
	bool isCoolTime_ = false;

	// -------------------------------------------------
	// ↓ Debug用
	// -------------------------------------------------

	// 編集フラグ
	bool isSelect_;
	bool isDelete_;

	// 選択カラー
	ImColor color_;
	ImColor baseColor_;

	std::string currentRunNodeName_;		// 現在実行中のNodeの名前
};

}

}

#ifndef AOENGINE_AI_NAMESPACE_ALIAS
#define AOENGINE_AI_NAMESPACE_ALIAS
namespace AI = AOENGINE::AI;
#endif
