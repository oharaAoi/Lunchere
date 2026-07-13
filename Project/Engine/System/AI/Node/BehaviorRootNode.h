#pragma once
// engine
#include "Engine/System/AI/Node/BaseBehaviorNode.h"

namespace AOENGINE {

namespace AI {

/// <summary>
/// Treeの根本となるクラス
/// </summary>
class BehaviorRootNode :
	public BaseBehaviorNode {
public: // constructor

	BehaviorRootNode();
	~BehaviorRootNode() = default;

public: // public method

	/// <summary>
	/// jsonに変換
	/// </summary>
	/// <returns></returns>
	json ToJson() override;

	/// <summary>
	/// 実行する
	/// </summary>
	/// <returns></returns>
	BehaviorStatus Execute() override;

	/// <summary>
	/// ウェイト値の計算
	/// </summary>
	/// <returns></returns>
	float EvaluateWeight() override { return 0; }

	/// <summary>
	/// 編集処理
	/// </summary>
	void Debug_Gui() override;
	
	/// <summary>
	/// 実行中のNodeの名前を取得する
	/// </summary>
	/// <returns></returns>
	std::string RunNodeName() override;

	/// <summary>
	/// 重みのテーブルに表示する項目
	/// </summary>
	void WeightTableItem() override {};

	/// <summary>
	/// taskの初期化用意
	/// </summary>
	void ResetNode() override {};

};

}

}

#ifndef AOENGINE_AI_NAMESPACE_ALIAS
#define AOENGINE_AI_NAMESPACE_ALIAS
namespace AI = AOENGINE::AI;
#endif
