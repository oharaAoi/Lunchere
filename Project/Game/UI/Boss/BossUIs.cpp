#include "BossUIs.h"
#include "Engine/Lib/Math/MyMath.h"
#include "Engine/Lib/Json/JsonItems.h"
#include <limits>

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossUIs::Init(Boss* _boss, Player* _player) {
	SetName("BossUIs");
	pBoss_ = _boss;
	pPlayer_ = _player;

	// ----------------------
	// ↓ HPのUI
	// ----------------------

	healthArc_ = std::make_unique<BaseGaugeUI>();
	healthArc_->SetName("healthArc");
	healthArc_->Init("gauge_bg.png", "gauge_front.png");
	healthArc_->GetFront()->Load("BossUIs", "BossHealthArcFrontUI");
	healthArc_->GetBg()->Load("BossUIs", "BossHealthArcBgUI");

	// ----------------------
	// ↓ 耐久ゲージのUI
	// ----------------------

	postureStability_ = std::make_unique<PostureStability>();
	postureStability_->Init("BossUIs", "PostureStability");

	postureStabilityArc_ = std::make_unique<BaseGaugeUI>();
	postureStabilityArc_->SetName("postureStabilityArc");
	postureStabilityArc_->Init("gauge_bg.png", "gauge_front.png");
	postureStabilityArc_->GetFront()->Load("BossUIs", "postureStabilityArcFrontUI");
	postureStabilityArc_->GetBg()->Load("BossUIs", "postureStabilityArcBgUI");

	// ----------------------
	// ↓ stun関連
	// ----------------------

	AOENGINE::Canvas2d* canvas = Engine::GetCanvas2d();
	stunPromote_ = canvas->AddSprite("stun.png", "stunPromote");
	stunPromote_->Load("BossUIs", "stunPromote");
	stunPromote_->GetTransform()->SetParent(postureStabilityArc_->GetFront()->GetMatrix());

	SetIsEnable(false);

	AddChild(healthArc_.get());
	AddChild(postureStability_.get());
	AddChild(postureStabilityArc_.get());
	AddChild(stunPromote_);
	
	AOENGINE::EditorWindows::AddObjectWindow(this, "BossUIs");
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void BossUIs::Update(const Math::Vector2& _reticlePos) {
	const BaseEnemy::BaseParameter& bossParam = pBoss_->GetBaseParameter();
	const BaseEnemy::BaseParameter& bossInitParam = pBoss_->GetInitBaseParameter();

	stunPromote_->SetIsActive(false);

	// ----------------------
	// ↓ hpゲージの更新
	// ----------------------
	float fillAmount = bossParam.health / bossInitParam.health;
	
	healthArc_->SetFillAmount(fillAmount);
	healthArc_->SetPos(_reticlePos);
	healthArc_->Update();

	// ----------------------
	// ↓ 耐久度ゲージの更新
	// ----------------------
	if (pBoss_->GetPulseArmor()->GetIsAlive()) {
		// armorの表示
		postureStability_->SetGaugeType(GaugeType::Armor);
		postureStability_->Update(pBoss_->GetPulseArmor()->ArmorDurability());

		postureStabilityArc_->SetFillAmount(pBoss_->GetPulseArmor()->ArmorDurability());

	} else if(pBoss_->GetIsStun()) {
		fillAmount = 1.0f - pBoss_->GetStunRemainingTime();

		// Stun時の表示
		postureStability_->SetGaugeType(GaugeType::Stun);
		postureStability_->Update(fillAmount);

		// 円ゲージの更新
		postureStabilityArc_->SetFillAmount(fillAmount);

		// stunの文字を表示
		stunPromote_->SetIsActive(true);
		stunPromote_->SetColor(postureStability_->GetFront()->GetColor());
		stunPromote_->Update();

	} else {
		// 通常時の表示
		postureStability_->SetGaugeType(GaugeType::Posturebility);
		postureStability_->Update(bossParam.postureStability / bossInitParam.postureStability);

		postureStabilityArc_->SetFillAmount(bossParam.postureStability / bossInitParam.postureStability);
	}

	// 姿勢制御の円ゲージの更新
	postureStabilityArc_->GetFront()->SetColor(postureStability_->GetFront()->GetColor());
	postureStabilityArc_->SetPos(_reticlePos);
	postureStabilityArc_->Update();

	postureStability_->SetIsEnable(false);
}

void BossUIs::SetIsEnable(bool flag) {
	healthArc_->SetIsEnable(flag);
	postureStability_->SetIsEnable(flag);
	postureStabilityArc_->SetIsEnable(flag);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集
///////////////////////////////////////////////////////////////////////////////////////////////

void BossUIs::Debug_Gui() {
}

void BossUIs::AlertUpdate() {
	// 警告
	for (auto it = attackAlertList_.begin(); it != attackAlertList_.end();) {
		if ((*it)->IsDestroy()) {
			it = attackAlertList_.erase(it);
		} else {
			++it;
		}
	}

	for (auto& alert : attackAlertList_) {
		alert->Update();
	}
}

void BossUIs::PopAlert(const Math::Vector3& _targetPos, const Math::Vector3& _attackerPos) {
	// 前方方向ベクトル
	Math::Vector3 forward = pPlayer_->GetTransform()->GetRotate().MakeForward();
	forward.y = 0;
	forward = forward.Normalize();

	// 右方向ベクトル
	Math::Vector3 right = pPlayer_->GetTransform()->GetRotate().MakeRight();
	right.y = 0;
	right = right.Normalize();

	// Targetへのベクトル
	Math::Vector3 toEnemy = (_attackerPos - _targetPos).Normalize();

	// 4方向を用意
	std::array<std::pair<AttackAlertDirection, Math::Vector3>, 4> dirs = { {
		{AttackAlertDirection::Front, forward},
		{AttackAlertDirection::Back,  forward * -1.0f},
		{AttackAlertDirection::Right, right},
		{AttackAlertDirection::Left,  right * -1.0f}
	} };

	// Dot値が最大の方向を選ぶ
	float bestDot = std::numeric_limits<float>::lowest();
	AttackAlertDirection bestDir = AttackAlertDirection::Front;

	for (auto& [dir, vec] : dirs) {
		float dot = Math::Vector3::Dot(toEnemy, vec);
		if (dot > bestDot) {
			bestDot = dot;
			bestDir = dir;
		}
	}

	auto& alert = attackAlertList_.emplace_back(std::make_unique<EnemyAttackAlert>());
	alert->Init(bestDir);
}
