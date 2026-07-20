#pragma once
// c++
#include <memory>
// game
#include "Game/Actor/Enemy/BaseEnemy.h"
#include "Game/UI/BaseGaugeUI.h"

/// <summary>
/// ターゲット中の敵情報を表示するUI
/// </summary>
class TargetUI {
public: // コンストラクタ

	TargetUI() = default;
	~TargetUI() = default;

public:

	/// <summary>
	/// 初期化関数
	/// </summary>
	void Init();

	void Update(const Math::Vector2& _reticlePos, BaseEnemy* targetEnemy);

	void SetIsEnable(bool flag);
	
private:

	std::unique_ptr<BaseGaugeUI> healthArc_;

};

