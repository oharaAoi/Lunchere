#pragma once
#include <functional>
#include <string>
#include <variant>
#include "Engine/Lib/Math/MathStructures.h"
#include "Engine/Module/Components/WorldTransform.h"
#include "Engine/Module/Components/Attribute/AttributeGui.h"
#include "Engine/Module/Components/IComponent.h"

enum class CollisionFlags {
	None = 0b00,
	Enter = 0b01,
	Exit = 0b10,
	Stay = 0b11,
};

enum class ColliderShape {
	Sphere,
	AABB,
	OBB,
	Line
};

namespace AOENGINE {

/// <summary>
/// Colliderの基底クラス
/// </summary>
class BaseCollider :
	public AOENGINE::AttributeGui, 
	public IComponent {
public:

	BaseCollider();
	virtual ~BaseCollider() = default;

	virtual void Init(const std::string& categoryName, ColliderShape shape) = 0;
	virtual void Update(const Math::QuaternionSRT& srt) = 0;
	virtual void Draw() const = 0;

	virtual void Debug_Gui() override;

public:

	/// <summary>
	/// 状態の変更
	/// </summary>
	void SwitchCollision(BaseCollider* partner);

	/// <summary>
	/// 当たっている相手を削除する
	/// </summary>
	/// <param name="partner"></param>
	void DeletePartner(BaseCollider* partner);

	void SetLocalPos(const Math::Vector3& pos) { localSRT_.translate = pos; }

	// --------------- 機能しているかの設定・取得 -------------- //
	void SetIsActive(bool isActive) { isActive_ = isActive; }
	bool GetIsActive() const { return isActive_; }

	// --------------- 移動するかどうか(押し戻すか)の設定・取得 -------------- //
	void SetIsStatic(bool isStatic) { isStatic_ = isStatic; }
	bool GetIsStatic() const { return isStatic_; }

	void SetIsTrigger(bool _isTrigger) { isTrigger_ = _isTrigger; }
	bool GetIsTrigger() const { return isTrigger_; }

	// --------------- categoryの設定・取得 -------------- //
	void SetLayerBit(uint32_t bit) { layerBit_ = bit; }
	uint32_t GetLayerBit() const { return layerBit_; }

	// --------------- maskの設定・取得 -------------- //
	void SetTarget(const std::string& id);
	void SetCollisionMaskBit(uint32_t bit) { collisionMaskBit_ |= bit; }
	uint32_t GetCollisionMaskBit() const { return collisionMaskBit_; }

	void SetCategory(const std::string& category);
	const std::string& GetCategoryName() const { return categoryName_; }

	// --------------- shapeの設定・取得 -------------- //
	void SetShape(const std::variant<Math::Sphere, Math::AABB, Math::OBB, Math::Line>& shape) { shape_ = shape; }
	const std::variant<Math::Sphere, Math::AABB, Math::OBB, Math::Line>& GetShape() const { return shape_; }

	// --------------- stateの設定・取得 -------------- //
	void SetCollisionState(int stateBit) { collisionState_ = stateBit; }
	int GetCollisionState() const { return collisionState_; }

	// ------------ 半径の設定・取得 ------------ // 
	void SetRadius(const float& radius) { std::get<Math::Sphere>(shape_).radius = radius; }
	float GetRadius() const { return std::get<Math::Sphere>(shape_).radius; }

	// ------------ Colliderの中心座標 ------------ // 
	const Math::Vector3& GetCenterPos() const { return centerPos_; }

	// ------------ 貫通対策 ------------ // 
	void SetPenetrationPrevention(bool isFlag) { penetrationPrevention_ = isFlag; }
	bool GetPenetrationPrevention() const { return penetrationPrevention_; }

	void SetPushBackDirection(const Math::Vector3& dire);
	const Math::Vector3& GetPushBackDirection() const { return pushbackDire_; }

	void SetOnCollision(std::function<void(BaseCollider*)> callback) {
		onCollision_ = callback;
	}

	void OnCollision(BaseCollider* other);

protected:

	bool isActive_ = false;
	bool isStatic_ = true;		// 移動するかどうか(押し戻すか)
	bool isTrigger_ = false;	// 判定だけを行うか

	// カテゴリ
	uint32_t layerBit_ = 0; // 自分が属しているカテゴリ
	uint32_t collisionMaskBit_ = 0;     // 誰と衝突してもいいかのマスク
	std::string categoryName_ = "None";

	// 形状
	std::variant<Math::Sphere, Math::AABB, Math::OBB, Math::Line> shape_;
	// 当たり判定の状態
	int collisionState_ = 0;
	// Colliderの中心座標
	Math::Vector3 centerPos_ = CVector3::ZERO;

	Math::QuaternionSRT localSRT_ = Math::QuaternionSRT();

	std::unordered_map<BaseCollider*, int> collisionPartnersMap_;

	// 貫通対策
	bool penetrationPrevention_ = false;	// 貫通対策を行うかどうか
	Math::Vector3 pushbackDire_ = CVector3::ZERO;

	// 汎用用
	std::function<void(BaseCollider*)> onCollision_;

};

}
