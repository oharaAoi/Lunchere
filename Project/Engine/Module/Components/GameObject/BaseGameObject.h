#pragma once
// c++
#include <list>
#include <vector>
#include <string>
#include <memory>
#include <utility>
#include <algorithm>
#include <unordered_map>
// engine
#include "Engine/Core/Engine.h"
#include "Engine/Lib/Math/MyMatrix.h"
#include "Engine/Lib/Color.h"
#include "Engine/System/ShaderGraph/ShaderGraph.h"
#include "Engine/Module/Components/GameObject/Model.h"
#include "Engine/Module/Components/Materials/BaseMaterial.h"
#include "Engine/Module/Components/GameObject/ISceneObject.h"
#include "Engine/Module/Components/WorldTransform.h"
#include "Engine/Module/Components/Animation/Animator.h"
#include "Engine/Module/Components/Collider/BaseCollider.h"
#include "Engine/Module/Components/Physics/Rigidbody.h"
#include "Engine/Module/Components/IComponent.h"

namespace AOENGINE {

/// <summary>
/// MeshやMaterialを持ったGameObject
/// </summary>
class BaseGameObject :
	public ISceneObject {
public: // constructor

	BaseGameObject() = default;
	virtual ~BaseGameObject();

public: // public method

	// 終了処理
	void Finalize();
	// 初期化処理
	void Init() override;
	// 更新処理
	void Update() override;
	// 影の描画
	void PreDraw() const override;
	// 描画処理
	void Draw() const override;
	// 行列の更新
	void UpdateMatrix();
	// 後から更新する処理
	void PostUpdate() override;

	// 編集処理
	void Debug_Gui() override;
	// Gizumo表示
	void Manipulate(const ImVec2& windowSize, const ImVec2& imagePos);

private: // private method

	void AddComponent();

public: // accessor method

	template<class T>
	T* GetComponent() {
		for (auto& component : components_) {
			if (auto result = dynamic_cast<T*>(component.get())) {
				return result;
			}
		}
		return nullptr;
	}

	template<class T, class... Args>
	T* AddComponent(Args&&... args) {
		auto component = std::make_unique<T>(std::forward<Args>(args)...);
		T* ptr = component.get();

		components_.push_back(std::move(component));
		return ptr;
	}

	void SetObject(const std::string& _objName, MaterialType _type = MaterialType::Normal);

	void SetParent(BaseGameObject* parent);

	AOENGINE::Model* GetModel() { return model_; }

	AOENGINE::WorldTransform* GetTransform() { return transform_; }

	Math::Vector3 GetPosition() const { return worldPos_; }

	void SetEnableShadow(bool _flag) { enableShadow_ = _flag; }

	void SetIsReflection(bool isReflection) { isReflection_ = isReflection; }

	void SetIsRendering(bool _isRendering) { isRendering_ = _isRendering; }

	// -------------------------------------------------
	// ↓ Material関連
	// -------------------------------------------------

	void SetColor(const AOENGINE::Color& color);

	void SetIsLighting(bool isLighting);

	void SetTexture(const std::string& path);

	void SetShaderGraph(AOENGINE::ShaderGraph* _shaderGraph);

	void SetMaterial(MaterialType _type);

	const std::unordered_map<std::string, std::unique_ptr<AOENGINE::BaseMaterial>>& GetMaterials() const { return materials; }

	// -------------------------------------------------
	// ↓ Animation関連
	// -------------------------------------------------

	void SetAnimator(const std::string& directoryPath, const std::string& objName, bool isSkinning, bool isLoop, bool isControlScript);

	AOENGINE::Animator* GetAnimator() { return animetor_.get(); }

	// -------------------------------------------------
	// ↓ Collider関連
	// -------------------------------------------------

	AOENGINE::BaseCollider* GetCollider(const std::string& name);
	AOENGINE::BaseCollider* SetCollider(const std::string& categoryName, ColliderShape shape);
	void AddCollider(AOENGINE::BaseCollider* _collider, const std::string& categoryName, ColliderShape shape);

	void SetPhysics();
	AOENGINE::Rigidbody* GetRigidbody() { return rigidbody_; }

protected:

	// componts
	std::list<std::unique_ptr<IComponent>> components_;

	AOENGINE::Model* model_ = nullptr;
	std::unordered_map<std::string, std::unique_ptr<AOENGINE::BaseMaterial>> materials;

	std::unique_ptr<AOENGINE::Animator> animetor_ = nullptr;

	std::vector<AOENGINE::BaseCollider*> colliders_;
	AOENGINE::WorldTransform* transform_ = nullptr;
	AOENGINE::Rigidbody* rigidbody_ = nullptr;

	Math::Vector3 worldPos_ = { 1.0f, 1.0f, 1.0f };

	bool isAnimation_ = false;

	bool isReflection_;
	bool isRendering_;

	// 他クラス情報
	AOENGINE::BaseGameObject* pParentObj_;

	bool enableShadow_ = true;
};

}