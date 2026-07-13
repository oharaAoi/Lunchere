#pragma once
#include <string>
#include <nlohmann/json.hpp>
#include "Engine/System/AI/State/BlackboardValue.h"
#include "Engine/System/AI/State/Blackboard.h"

namespace AOENGINE {

namespace AI {

/// <summary>
/// if文となるクラス
/// </summary>
class Condition {
public: // constructor

	Condition() = default;
	~Condition() = default;

public: // public method

	/// <summary>
	/// 実行処理
	/// </summary>
	/// <param name="_state"></param>
	/// <returns></returns>
	bool Execute(Blackboard* _state);

	/// <summary>
	/// 判断する
	/// </summary>
	bool Compare(const BlackboardValue& lhs, const BlackboardValue& rhs, const std::string& op);

	/// <summary>
	/// 編集処理
	/// </summary>
	/// <param name="_state"></param>
	void Debug_Gui(Blackboard* _state);

	/// <summary>
	/// jsonに変換する
	/// </summary>
	/// <returns></returns>
	nlohmann::json ToJson();

	/// <summary>
	/// jsonをNodeに適応させる
	/// </summary>
	/// <param name="_jsonData"></param>
	void FromJson(const nlohmann::json& _jsonData);

private: // private variable

	static const int32_t kOperatorCount_ = 6;
	const char* conditionOps[kOperatorCount_] = {
	"==", "!=", ">", "<", ">=", "<="
	};
	int32_t opIndex_ = 0;
	int32_t leftKeyIndex_ = 0;
	int32_t rightKeyIndex_ = 0;

	std::string leftKey_ = "";
	std::string rightKey_ = "";

};

}

}

#ifndef AOENGINE_AI_NAMESPACE_ALIAS
#define AOENGINE_AI_NAMESPACE_ALIAS
namespace AI = AOENGINE::AI;
#endif
