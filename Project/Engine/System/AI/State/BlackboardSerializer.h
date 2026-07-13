#pragma once
#include <string>
#include <unordered_map>
#include <nlohmann/json.hpp>
#include "Engine/System/AI/State/BlackboardValue.h"

using json = nlohmann::json;

namespace AOENGINE {

namespace AI {

/// <summary>
/// WroldStateを保存するためのクラス
/// </summary>
class BlackboardSerializer {
public: // constructor

	BlackboardSerializer() = default;
	~BlackboardSerializer() = default;

public: // public method

	static bool Save(const std::string& _filePath, const std::unordered_map<std::string, BlackboardValue>& _map);

	static bool Load(const std::string& _filePath, std::unordered_map<std::string, BlackboardValue>& _map);

};

}

}

#ifndef AOENGINE_AI_NAMESPACE_ALIAS
#define AOENGINE_AI_NAMESPACE_ALIAS
namespace AI = AOENGINE::AI;
#endif
