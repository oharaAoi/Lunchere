#include "BaseGameObject.h"
#include "Engine/Module/Components/Collider/SphereCollider.h"
#include "Engine/Module/Components/Collider/BoxCollider.h"
#include "Engine/Module/Components/Collider/LineCollider.h"
#include "Engine/Module/Components/Materials/Material.h"
#include "Engine/Module/Components/Materials/PBRMaterial.h"
#include "Engine/System/Collision/ColliderCollector.h"
#include "Engine/System/Manager/ModelManager.h"
#include "Engine/Lib/GameTimer.h"
#include "Engine/Render/Render.h"

using namespace AOENGINE;

BaseGameObject::~BaseGameObject() {
	Finalize();
}

void BaseGameObject::Finalize() {
	if (transform_ != nullptr) {
		transform_->Finalize();
		transform_ = nullptr;
	}
	if (animetor_ != nullptr) {
		animetor_->Finalize();
		animetor_ = nullptr;
	}
	if (!colliders_.empty()) {
		for (uint32_t index = 0; index < colliders_.size(); ++index) {
			ColliderCollector::GetInstance()->RemoveCollider(colliders_[index]);
			colliders_[index] = nullptr;
		}
	}
	materials.clear();

	components_.clear();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　初期化処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void BaseGameObject::Init() {
	transform_ = AddComponent<WorldTransform>();
	transform_->Init();
	animetor_ = nullptr;

	isDestroy_ = false;
	isRendering_ = true;

	transform_->Update();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　更新処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void BaseGameObject::Update() {
	transform_->PostUpdate();
	// アニメーションが設定されていない場合はTransformのみ更新
	if (animetor_ == nullptr) {
		UpdateMatrix();
		return;
	}

	// アニメーションの制御をスクリプトで行っていない場合
	if (!animetor_->GetIsControlScript()) {
		animetor_->Update();

		if (animetor_->GetIsSkinning()) {
			Engine::SetPipelineCS("Skinning.json");
			for (uint32_t index = 0; index < model_->GetMeshsNum(); ++index) {
				Engine::SetSkinning(animetor_->GetSkinning(index));
			}
		} else {
			UpdateMatrix();
			transform_->Update(animetor_->GetAnimationMat());
			return;
		}
	} else { 
		if (animetor_->GetIsSkinning()) {
			Engine::SetPipelineCS("Skinning.json");
			for (uint32_t index = 0; index < model_->GetMeshsNum(); ++index) {
				Engine::SetSkinning(animetor_->GetSkinning(index));
			}
		}
	}

	UpdateMatrix();
}

void BaseGameObject::UpdateMatrix() {
	// 移動量や追加する
	if (rigidbody_ != nullptr) {
		transform_->Translate(rigidbody_->GetMoveForce());
		rigidbody_->Update();
		transform_->Translate(rigidbody_->GetVelocity(), AOENGINE::GameTimer::DeltaTime());
	}

	// transformの更新
	transform_->Update();

	// colliderの更新
	if (!colliders_.empty()) {
		for (uint32_t index = 0; index < colliders_.size(); ++index) {
			colliders_[index]->Update(Math::QuaternionSRT{
				.scale = transform_->GetScale(),
				.rotate = transform_->GetWorldRotate(),
				.translate = transform_->GetWorldPos() }
			);
		}
	}

	worldPos_ = transform_->GetWorldMatrix().GetPosition();

	for (auto& material : materials) {
		material.second->Update();
	}
}

void BaseGameObject::PostUpdate() {
	// colliderから押し戻し量を取得する
	if (rigidbody_ != nullptr) {
		for (uint32_t index = 0; index < colliders_.size(); ++index) {
			if (!colliders_[index]->GetIsStatic()) {
				rigidbody_->SetPushbackForce(colliders_[index]->GetPushBackDirection());
			}
		}
		transform_->Translate(rigidbody_->GetPushbackForce());
	}

	// 押し戻し後の更新を行なう
	transform_->Update();
	worldPos_ = transform_->GetWorldMatrix().GetPosition();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　影の描画
//////////////////////////////////////////////////////////////////////////////////////////////////

void BaseGameObject::PreDraw() const {
	if (!enableShadow_) { return; }
	if (model_ == nullptr) { return; }
	Engine::SetPipeline(PSOType::Object3d, "Object_ShadowMap.json");
	Pipeline* pso = Engine::GetLastUsedPipeline();
	for (uint32_t index = 0; index < model_->GetMeshsNum(); ++index) {
		if (animetor_ == nullptr || !animetor_->GetIsSkinning()) {
			AOENGINE::Render::SetShadowMesh(pso, model_->GetMesh(index), transform_, model_->GetMesh(index)->GetVBV());
		} else {
			AOENGINE::Render::SetShadowMesh(pso, model_->GetMesh(index), transform_, animetor_->GetSkinning(index)->GetVBV());
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　描画処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void BaseGameObject::Draw() const {
	// objectが設定されていなかったら描画処理は呼ばない
	if (model_ == nullptr) {
		return;
	}

	if (!isRendering_) {
		return;
	}

	// 鏡面反射をする場合の描画
	if (isReflection_) {
		for (uint32_t index = 0; index < model_->GetMeshsNum(); index++) {
			if (materials.size() > index) {
				Pipeline* pso = Engine::GetLastUsedPipeline();
				Mesh* pMesh = model_->GetMesh(index);
				AOENGINE::Render::DrawEnvironmentModel(pso, pMesh, materials.at(pMesh->GetUseMaterial()).get(), transform_);
			}
		}
		return;
	}

	Pipeline* pso = Engine::GetLastUsedPipeline();
	if (animetor_ == nullptr || !animetor_->GetIsSkinning()) {
		AOENGINE::Render::DrawModel(pso, model_, transform_, materials);
	} else {
		AOENGINE::Render::DrawModel(pso, model_, transform_, animetor_->GetSkinningArray(), materials);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　Colliderを設定する
//////////////////////////////////////////////////////////////////////////////////////////////////

BaseCollider* BaseGameObject::GetCollider(const std::string& name) {
	for (uint32_t index = 0; index < colliders_.size(); ++index) {
		if (colliders_[index]->GetCategoryName() == name) {
			return colliders_[index];
		}
	}
	return nullptr;
}

BaseCollider* BaseGameObject::SetCollider(const std::string& categoryName, ColliderShape shape) {
	if (shape == ColliderShape::Sphere) {
		BaseCollider* collider = AddComponent<SphereCollider>();
		AddCollider(collider, categoryName, shape);
		colliders_.push_back(collider);
		return collider;
	} else if (shape == ColliderShape::AABB || shape == ColliderShape::OBB) {
		BaseCollider* collider = AddComponent<BoxCollider>();
		AddCollider(collider, categoryName, shape);
		colliders_.push_back(collider);
		return collider;
	} else if (shape == ColliderShape::Line) {
		BaseCollider* collider = AddComponent<LineCollider>();
		AddCollider(collider, categoryName, shape);
		colliders_.push_back(collider);
		return collider;
	}
	return nullptr;
}

void BaseGameObject::AddCollider(BaseCollider* _collider, const std::string& categoryName, ColliderShape shape) {
	_collider->Init(categoryName, shape);
	_collider->SetName(categoryName);
	_collider->SetCategory(categoryName);
	ColliderCollector::AddCollider(_collider);

	_collider->Update(Math::QuaternionSRT{ .scale = transform_->GetScale(),
						  .rotate = transform_->GetRotate(),
						  .translate = transform_->GetTranslate() }
	);
}

void BaseGameObject::SetPhysics() {
	rigidbody_ = AddComponent<Rigidbody>();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　modelを設定する
//////////////////////////////////////////////////////////////////////////////////////////////////

void BaseGameObject::SetObject(const std::string& _objName, MaterialType _type) {
	model_ = nullptr;
	materials.clear();

	model_ = ModelManager::GetInstance()->GetModel(_objName);
	for (const auto& material : model_->GetMaterialData()) {
		if (_type == MaterialType::Normal) {
			materials[material.first] = std::make_unique<Material>();
		} else if (_type == MaterialType::PBR) {
			materials[material.first] = std::make_unique<PBRMaterial>();
		}
		materials[material.first]->Init();
		materials[material.first]->SetMaterialData(material.second);
	}
}

void AOENGINE::BaseGameObject::SetMaterial(MaterialType _type) {
	for (const auto& material : model_->GetMaterialData()) {
		if (_type == MaterialType::Normal) {
			materials[material.first] = std::make_unique<Material>();
		} else if (_type == MaterialType::PBR) {
			materials[material.first] = std::make_unique<PBRMaterial>();
		}
		materials[material.first]->Init();
		materials[material.first]->SetMaterialData(material.second);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　親を設定する
//////////////////////////////////////////////////////////////////////////////////////////////////

void BaseGameObject::SetParent(BaseGameObject* parent) {
	pParentObj_ = parent;
	transform_->SetParent(parent->GetTransform()->GetWorldMatrix());
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　アニメーションを設定する
//////////////////////////////////////////////////////////////////////////////////////////////////

void BaseGameObject::SetAnimator(const std::string& directoryPath, const std::string& objName, bool isSkinning, bool isLoop, bool isControlScript) {
	animetor_ = std::make_unique<Animator>();
	animetor_->LoadAnimation(directoryPath, objName, model_, isSkinning, isLoop, isControlScript);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　materialの色を変える
//////////////////////////////////////////////////////////////////////////////////////////////////

void BaseGameObject::SetColor(const Color& color) {
	for (auto& material : materials) {
		material.second->SetColor(color);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　lightingの設定を変更する
//////////////////////////////////////////////////////////////////////////////////////////////////

void BaseGameObject::SetIsLighting(bool isLighting) {
	for (auto& material : materials) {
		material.second->SetIsLighting(isLighting);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　materialのtextureを変更する
//////////////////////////////////////////////////////////////////////////////////////////////////

void BaseGameObject::SetTexture(const std::string& path) {
	for (auto& material : materials) {
		material.second->SetAlbedoTexture(path);
	}
}

void BaseGameObject::SetShaderGraph(ShaderGraph* _shaderGraph) {
	for (auto& material : materials) {
		material.second->SetShaderGraph(_shaderGraph);
	}
}

void BaseGameObject::Debug_Gui() {
	ImGui::Checkbox("enableShadow", &enableShadow_);
	transform_->Debug_Gui();

	if (ImGui::CollapsingHeader("Model")) {
		if (model_) {
			ImGui::Text(model_->GetName().c_str());
		} else {
			ImGui::Text("null");
		}
		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_HANDLE")) {
				const AssetHandle& handle = *static_cast<const AssetHandle*>(payload->Data);
				if (handle.type == AssetType::Model) {
					Model* model = ModelManager::GetInstance()->SearchModel(handle.id);
					if (model) {
						SetObject(model->GetName());
					}
				}
			}
			ImGui::EndDragDropTarget();
		}
	}

	int index = 0;
	if (ImGui::CollapsingHeader("Material")) {
		for (auto& material : materials) {
			std::string guiId = "material_" + std::to_string(index);
			if (ImGui::TreeNode(guiId.c_str())) {
				material.second->Debug_Gui();
				ImGui::TreePop();
			}
			index++;
		}
	}

	if (!colliders_.empty()) {
		if (ImGui::CollapsingHeader("Colliders")) {
			for (uint32_t oi = 0; oi < colliders_.size(); ++oi) {
				std::string tag = colliders_[oi]->GetCategoryName();
				if (ImGui::TreeNode(tag.c_str())) {
					colliders_[oi]->Debug_Gui();
					ImGui::TreePop();
				}
			}
		}
	}

	if (rigidbody_) {
		if (ImGui::CollapsingHeader("Rigidbody")) {
			rigidbody_->Debug_Gui();
		}
	}

	if (animetor_ != nullptr) {
		if (ImGui::CollapsingHeader("Animator")) {
			animetor_->Debug_Gui();
		}
	}

	ImGui::Separator();
	AddComponent();
}

void BaseGameObject::Manipulate(const ImVec2& windowSize, const ImVec2& imagePos) {
	transform_->Manipulate(windowSize, imagePos);
}

void AOENGINE::BaseGameObject::AddComponent() {
	if (ImGui::Button("Add Component")) {
		ImGui::OpenPopup("AddComponentPopup");
	}

	if (ImGui::BeginPopup("AddComponentPopup")) {
		if (ImGui::BeginMenu("Physics")) {
			if (ImGui::MenuItem("Box Collider")) {
				SetCollider("Default", ColliderShape::AABB);
			}
			if (ImGui::MenuItem("Sphere Collider")) {
				SetCollider("Default", ColliderShape::Sphere);
			}
			if (ImGui::MenuItem("Rigid Body")) {
				SetPhysics();
			}
			ImGui::EndMenu();
		}
		ImGui::EndPopup();
	}
}
