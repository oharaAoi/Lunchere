#pragma once
// c++
#include <functional>
#include <string>
// engine
#include "Engine/System/AI/Node/BaseBehaviorNode.h"
#include "Engine/System/AI/UtilityAI/UtilityEvaluator.h"
#include "Engine/Lib/GameTimer.h"
#include "Engine/Utilities/Timer.h"

namespace AI {

/// <summary>
/// nodeのtask
/// </summary>
/// <typeparam name="OwnerType"></typeparam>
template<typename OwnerType>
/// <summary>
/// タスクノード基底クラス
/// </summary>
class BaseTaskNode :
	public BaseBehaviorNode {
public: // constructor

	BaseTaskNode();
	virtual ~BaseTaskNode() override = default;

public: // public method

	/// <summary>
	/// jsonへ変換
	/// </summary>
	/// <returns></returns>
	json ToJson() override;

	/// <summary>
	/// jsonから適応させる
	/// </summary>
	/// <param name="_jsonData"></param>
	void FromJson(const json& _jsonData) override;

	/// <summary>
	/// 実行処理
	/// </summary>
	/// <returns></returns>
	virtual BehaviorStatus Execute() override = 0;

	/// <summary>
	/// weightを算出
	/// </summary>
	/// <returns></returns>
	float EvaluateWeight() override = 0;

	/// <summary>
	/// 実行中のNodeの名前を取得する
	/// </summary>
	/// <returns></returns>
	std::string RunNodeName() override;

	/// <summary>
	/// 重みのテーブルに表示する項目
	/// </summary>
	void WeightTableItem() override;

	// 初期化処理
	virtual void Init() = 0;
	// 更新処理
	virtual void Update() = 0;
	// 終了処理
	virtual void End() = 0;
	// 終了確認
	virtual bool IsFinish() = 0;
	// 実行できるを判別
	virtual bool CanExecute() = 0;
	// 編集処理
	virtual void Debug_Gui() override;
	// 行動を行う
	virtual BehaviorStatus Action();
	// 積極性のスコアを計算
	float CalcAggressionScore(float _aggression);

private: // private variable

	// 待機処理
	bool Wait();

	// taskの初期化用意
	void ResetNode() override;

public: // accessor

	void SetTarget(OwnerType* owner) { pTarget_ = owner; }

protected: // protected variable

	OwnerType* pTarget_ = nullptr;

	std::function<void()> action_;

	float taskTimer_ = 0.0f;

	float weight_ = 0.1f;				// 重み
 	float aggressionAffinity_ = 0.1f;	// 攻撃親和性 

	AOENGINE::Timer waitTimer_;

	UtilityEvaluator evaluator_;
};

// =================================================================================================================== //

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　コンストラクタ
//////////////////////////////////////////////////////////////////////////////////////////////////

template<typename OwnerType>
inline BaseTaskNode<OwnerType>::BaseTaskNode() {
	type_ = NodeType::Task;
	color_ = ImColor(153, 102, 204);
	baseColor_ = color_;
	isLeafNode_ = true;
	waitTimer_.targetTime_ = 1;
	evaluator_ = UtilityEvaluator();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　jsonに変換する
//////////////////////////////////////////////////////////////////////////////////////////////////

template<typename OwnerType>
inline json BaseTaskNode<OwnerType>::ToJson() {
	json item;
	item["name"] = node_.name;
	item["nodeType"] = static_cast<int>(type_);
	item["nodePos"] = json{ {"x", pos_.x}, {"y", pos_.y} };
	item["waitTime"] = waitTimer_.targetTime_;
	item["coolTime"] = coolTimer_.targetTime_;
	item["weight"] = weight_;
	item["aggressionAffinity"] = aggressionAffinity_;
	item["children"] = json::array();

	for (const auto& child : children_) {
		item["children"].push_back(child->ToJson());
	}
	return item;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　jsonからNodeの情報を取得する
//////////////////////////////////////////////////////////////////////////////////////////////////

template<typename OwnerType>
inline void BaseTaskNode<OwnerType>::FromJson(const json& _jsonData) {
	node_.name = _jsonData["name"];
	type_ = _jsonData["nodeType"];
	pos_ = Math::Vector2(_jsonData["nodePos"]["x"], _jsonData["nodePos"]["y"]);

	if (_jsonData.contains("waitTime")) {
		waitTimer_.targetTime_ = _jsonData["waitTime"].get<float>();
	}
	if (_jsonData.contains("coolTime")) {
		coolTimer_.targetTime_ = _jsonData["coolTime"].get<float>();
	}
	if (_jsonData.contains("weight")) {
		weight_ = _jsonData["weight"].get<float>();
	}
	if (_jsonData.contains("aggressionAffinity")) {
		aggressionAffinity_ = _jsonData["aggressionAffinity"].get<float>();
	}

	coolTimer_.timer_ = coolTimer_.targetTime_;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　デバックの表示
//////////////////////////////////////////////////////////////////////////////////////////////////

template<typename OwnerType>
inline void BaseTaskNode<OwnerType>::Debug_Gui() {
	ImGui::BulletText("Task Name : %s", node_.name.c_str());
	ImGui::DragFloat("weight", &weight_, 0.1f);
	ImGui::SliderFloat("wait_timer", &waitTimer_.timer_, 0.0f, waitTimer_.targetTime_);
	ImGui::DragFloat("行動後の待機時間", &waitTimer_.targetTime_, 0.1f);
	ImGui::SliderFloat("cool_timer", &coolTimer_.timer_, 0.0f, coolTimer_.targetTime_);
	ImGui::DragFloat("行動自体のクールタイム", &coolTimer_.targetTime_, 0.1f);
	evaluator_.Debug_Gui();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　行動を行う
//////////////////////////////////////////////////////////////////////////////////////////////////

template<typename OwnerType>
inline BehaviorStatus BaseTaskNode<OwnerType>::Action() {
 	if (isCoolTime_) {
		return BehaviorStatus::Failure;
	}

	// 非アクティブ状態なら初期化を行う
	if (state_ == BehaviorStatus::Success || state_ == BehaviorStatus::Inactive) {
		if (!CanExecute()) {
			return BehaviorStatus::Failure;
		}
		taskTimer_ = 0;
		waitTimer_.timer_ = 0;
		state_ = BehaviorStatus::Running;
		Init();
	}

	// 更新
	if (!IsFinish()) {
		Update();
	} else {
		if (Wait()) {
			ResetNode();
			state_ = BehaviorStatus::Success;
			return BehaviorStatus::Success;
		}
	}

	return BehaviorStatus::Running;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　待機処理
//////////////////////////////////////////////////////////////////////////////////////////////////

template<typename OwnerType>
inline bool BaseTaskNode<OwnerType>::Wait() {
	if (!waitTimer_.Run(AOENGINE::GameTimer::DeltaTime())) {
		return true;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　Nodeのリセット
//////////////////////////////////////////////////////////////////////////////////////////////////

template<typename OwnerType>
inline void BaseTaskNode<OwnerType>::ResetNode() {
	End();
	coolTimer_.timer_ = 0;
	state_ = BehaviorStatus::Inactive;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　積極性を計算する
//////////////////////////////////////////////////////////////////////////////////////////////////

template<typename OwnerType>
inline float BaseTaskNode<OwnerType>::CalcAggressionScore(float _aggression) {
	return std::lerp(1.0f - aggressionAffinity_, aggressionAffinity_, _aggression);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　実行中のNodeの名前を返す
//////////////////////////////////////////////////////////////////////////////////////////////////

template<typename OwnerType>
inline std::string BaseTaskNode<OwnerType>::RunNodeName() {
	return BaseRunNodeName();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　WeightTableに表示する重みの表示
//////////////////////////////////////////////////////////////////////////////////////////////////

template<typename OwnerType>
inline void BaseTaskNode<OwnerType>::WeightTableItem() {
	std::string name = "##weight" + GetName() + std::to_string(GetID());
	ImGui::DragFloat(name.c_str(), &weight_, 0.1f, 0.01f, 1.0f);
	ImGui::TableSetColumnIndex(2);
	name = "##aggression" + GetName() + std::to_string(GetID());
	ImGui::DragFloat(name.c_str(), &aggressionAffinity_, 0.1f, 0.01f, 1.0f);
}
}