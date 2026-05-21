#pragma once
#include <list>
#include <memory>
#include <cmath>
#define NOMINMAX
#define _USE_MATH_DEFINES
#include <math.h>
#include <algorithm>
#include "Engine/System/AI/Node/BaseBehaviorNode.h"
#include "Engine/System/AI/GoalOriented/IOrientedGoal.h"
#include "Engine/System/AI/CommentBox.h"

namespace AI {

/// <summary>
/// Treeを編集するためのクラス
/// </summary>
class BehaviorTreeEditor {
public: // data

	using ActionNode = std::function<std::unique_ptr<BaseBehaviorNode>()>;

	/// <summary>
	/// ドラッグ選択矩形情報
	/// </summary>
	struct DragRect {
		bool   dragging = false;
		ImVec2 startScreen;
		ImVec2 currentScreen;
	};

public: // constructor

	BehaviorTreeEditor() = default;
	~BehaviorTreeEditor();

public: // public method

	/// <summary>
	/// 終了処理
	/// </summary>
	void Finalize();

	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="directoryPath"></param>
	/// <param name="fileName"></param>
	void Init(const std::string& directoryPath, const std::string& fileName);

	/// <summary>
	/// 編集を行なう
	/// </summary>
	/// <param name="_nodeList"></param>
	/// <param name="_link"></param>
	/// <param name="_root"></param>
	/// <param name="_worldState"></param>
	/// <param name="_canTaskMap"></param>
	/// <param name="_goalArray"></param>
	void Edit(const std::string& name,
			  std::list<std::unique_ptr<BaseBehaviorNode>>& nodeList,
			  std::vector<Link>& link,
			  BaseBehaviorNode* root,
			  Blackboard* worldState,
			  const std::unordered_map<std::string, ActionNode>& creators,
			  const std::vector<std::shared_ptr<IOrientedGoal>>& goalArray);

	/// <summary>
	/// Editorで選択されているものを表示
	/// </summary>
	void EditSelect();

	/// <summary>
	/// Editor上でのコメントアウトを作成する
	/// </summary>
	/// <param name="_json"></param>
	void CreateCommentsFromJson(const json& json);

	/// <summary>
	/// コメントアウトをJson化する
	/// </summary>
	/// <param name="_json"></param>
	void CommentsToJson(json& json);

private: // private variable

	/// <summary>
	/// コメントNodeの作成
	/// </summary>
	void CreateCommentNode();
	
	/// <summary>
	/// コメントの更新
	/// </summary>
	void CommentFrame();

	/// <summary>
	/// 保存と読み込み
	/// </summary>
	/// <param name="_nodeList"></param>
	/// <param name="_link"></param>
	/// <param name="_root"></param>
	/// <param name="_worldState"></param>
	/// <param name="_canTaskMap"></param>
	/// <param name="_goalArray"></param>
	void SaveAndLoad(std::list<std::unique_ptr<BaseBehaviorNode>>& nodeList,
					 std::vector<Link>& link,
					 BaseBehaviorNode* root,
					 Blackboard* worldState,
					 const std::unordered_map<std::string, ActionNode>& creators,
					 const std::vector<std::shared_ptr<IOrientedGoal>>& goalArray);

	/// <summary>
	/// Node生成のwindowを表示
	/// </summary>
	/// <param name="_nodeList"></param>
	/// <param name="_worldState"></param>
	/// <param name="_canTaskMap"></param>
	/// <param name="_goalArray"></param>
	void CreateNodeWindow(std::list<std::unique_ptr<BaseBehaviorNode>>& nodeList,
						  Blackboard* worldState,
						  const std::unordered_map<std::string, ActionNode>& creators,
						  const std::vector<std::shared_ptr<IOrientedGoal>>& goalArray);

	/// <summary>
	/// セレクトされているNodeを確認する
	/// </summary>
	/// <param name="_nodeList"></param>
	void CheckSelectNode(std::list<std::unique_ptr<BaseBehaviorNode>>& nodeList);

	/// <summary>
	/// 削除するNodeを確認する
	/// </summary>
	/// <param name="_nodeList"></param>
	/// <param name="_root"></param>
	void CheckDeleteNode(std::list<std::unique_ptr<BaseBehaviorNode>>& nodeList, BaseBehaviorNode* root);

	/// <summary>
	/// 接続処理
	/// </summary>
	/// <param name="_nodeList"></param>
	/// <param name="_link"></param>
	/// <param name="_root"></param>
	void Connect(std::list<std::unique_ptr<BaseBehaviorNode>>& nodeList, std::vector<Link>& links, BaseBehaviorNode* root);

	/// <summary>
	/// 接続解除処理
	/// </summary>
	/// <param name="_nodeList"></param>
	/// <param name="_link"></param>
	/// <param name="_root"></param>
	void UnConnect(std::list<std::unique_ptr<BaseBehaviorNode>>& nodeList, std::vector<Link>& links, BaseBehaviorNode* root);

	/// <summary>
	/// Node描画
	/// </summary>
	/// <param name="_nodeList"></param>
	void DrawNode(std::list<std::unique_ptr<BaseBehaviorNode>>& nodeList);

	/// <summary>
	/// Nodeの検索
	/// </summary>
	/// <param name="_nodeList"></param>
	/// <param name="_pin"></param>
	/// <returns></returns>
	BaseBehaviorNode* FindNodeFromPin(std::list<std::unique_ptr<BaseBehaviorNode>>& nodeList, ax::NodeEditor::PinId pin);

private: // private variable

	// nodeEditorのポインタ
	ax::NodeEditor::EditorContext* context_ = nullptr;

	BaseBehaviorNode* selectNode_;
	ax::NodeEditor::NodeId selectId_;

	ImVec2 popupPos_;
	bool popupRequested_ = false;

	// コメントボックス
	std::list<std::unique_ptr<CommentBox>> commentBox_;

	DragRect drag_;

	std::string settingsFilePath_;
};

}