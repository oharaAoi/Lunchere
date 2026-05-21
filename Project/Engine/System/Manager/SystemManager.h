#pragma once
#include "Engine/System/AI/BehaviorTreeSystem.h"

namespace AOENGINE {

/// <summary>
/// システム管理クラス
/// </summary>
class SystemManager {
public: // コンストラクタ

	SystemManager() = default;
	~SystemManager() = default;

public:

	void Finalize();

	void Init();

	void Update();

private:

	AI::BehaviorTreeSystem* behaviorTree_;

};

}