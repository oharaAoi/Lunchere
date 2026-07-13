#pragma once
#include <list>
#include <memory>
#include "Engine/System/AI/BehaviorTree.h"

namespace AOENGINE {

namespace AI {

/// <summary>
/// Treeを管理するクラス
/// </summary>
class BehaviorTreeSystem {
public: // constructor

	BehaviorTreeSystem() = default;
	~BehaviorTreeSystem() = default;
	BehaviorTreeSystem(const BehaviorTreeSystem&) = delete;
	const BehaviorTreeSystem& operator=(const BehaviorTreeSystem&) = delete;

	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	/// <returns></returns>
	static BehaviorTreeSystem* GetInstance();

public: // public method

	/// <summary>
	/// 終了処理
	/// </summary>
	void Finalize();

	/// <summary>
	/// 初期化処理
	/// </summary>
	void Init();

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// 生成処理
	/// </summary>
	/// <returns></returns>
	BehaviorTree* Create();

	/// <summary>
	/// Editorを開く
	/// </summary>
	/// <param name="name"></param>
	void SetIsOpenEditor(const std::string& name);


public: // accessor

	void SetSelectNode(BaseBehaviorNode* _node) { selectNode_ = _node; }

private: // private variable

	std::list<std::unique_ptr<BehaviorTree>> trees_;

	BaseBehaviorNode* selectNode_ = nullptr;;

};

}

}

#ifndef AOENGINE_AI_NAMESPACE_ALIAS
#define AOENGINE_AI_NAMESPACE_ALIAS
namespace AI = AOENGINE::AI;
#endif
