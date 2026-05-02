#include "WorldTransform.h"
#include "Engine/System/Manager/ImGuiManager.h"
#include "Engine/Lib/Math/MyMatrix.h"
#include "Engine/Render/Render.h"
#include "Engine/System/Editor/Tool/ManipulateTool.h"
#include "Engine/Core/GraphicsContext.h"

using namespace AOENGINE;

int AOENGINE::WorldTransform::nextId_ = 0;

WorldTransform::WorldTransform() {
	id_ = nextId_;
	nextId_++;
}
WorldTransform::~WorldTransform() {
	Finalize();
}

void AOENGINE::WorldTransform::Finalize() {
	cBuffer_.Reset();
	data_ = nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　初期化処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void AOENGINE::WorldTransform::Init() {
	GraphicsContext* ctx = GraphicsContext::GetInstance();

	cBuffer_ = CreateBufferResource(ctx->GetDevice(), sizeof(AOENGINE::WorldTransformData));
	// データをマップ
	cBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&data_));

	// 値を初期化しておく
	srt_.scale = { 1.0f, 1.0f, 1.0f };
	srt_.rotate = Math::Quaternion();
	srt_.translate = { 0.0f, 0.0f, 0.0f };
	worldMat_ = Math::Matrix4x4::MakeUnit();

	moveQuaternion_ = Math::Quaternion();
	isBillboard_ = false;

	data_->matWorldPrev = Math::Matrix4x4::MakeUnit();
	data_->matWorld = Math::Matrix4x4::MakeUnit();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　更新処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void AOENGINE::WorldTransform::Update(const Math::Matrix4x4& mat) {
	srt_.rotate = moveQuaternion_ * srt_.rotate;
	srt_.rotate = srt_.rotate.Normalize();

	// -------------------------------------------------
	// ↓ world行列を生成
	// -------------------------------------------------

	Math::Matrix4x4 scaleMat = srt_.scale.MakeScaleMat();
	Math::Matrix4x4 rotateMat = srt_.rotate.MakeMatrix();
	if (isBillboard_) {
		rotateMat = Multiply(rotateMat, AOENGINE::Render::GetBillBordMat());
	}
	Math::Matrix4x4 translateMat = Math::Vector3(srt_.translate).MakeTranslateMat();

	worldMat_ = mat * Multiply(Multiply(scaleMat, rotateMat), translateMat);
	if (parentWorldMat_ != nullptr) {
		worldMat_ = worldMat_ * *parentWorldMat_;
	}

	// GPUに送るデータを更新
	data_->matWorld = worldMat_;
	data_->worldInverseTranspose = (worldMat_).Inverse().Transpose();

	preTranslate_ = srt_.translate;
	moveQuaternion_ = Math::Quaternion();
}

void AOENGINE::WorldTransform::PostUpdate() {
	data_->matWorldPrev = worldMat_;
}

void AOENGINE::WorldTransform::MoveVelocity(const Math::Vector3& velocity, float rotationSpeed) {
	srt_.translate += velocity;

	if (velocity.x != 0.0f || velocity.y != 0.0f) {
		Math::Quaternion rotate = Math::Quaternion::LookRotation(velocity.Normalize());
		srt_.rotate = Math::Quaternion::Slerp(srt_.rotate, rotate, rotationSpeed);
	}
}

void AOENGINE::WorldTransform::LookAt(const Math::Vector3& target, const Math::Vector3& up) {
	Math::Vector3 direction = target - srt_.translate;
	srt_.rotate = Math::Quaternion::LookRotation(direction.Normalize(), up);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　コマンドリストに送る
//////////////////////////////////////////////////////////////////////////////////////////////////

void AOENGINE::WorldTransform::BindCommandList(ID3D12GraphicsCommandList* commandList, UINT index) const {
	commandList->SetGraphicsRootConstantBufferView(index, cBuffer_->GetGPUVirtualAddress());
}

void AOENGINE::WorldTransform::Translate(const Math::Vector3& translate, float _deltaTime) {
	srt_.translate += translate * _deltaTime;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　編集
//////////////////////////////////////////////////////////////////////////////////////////////////

void AOENGINE::WorldTransform::Debug_Gui() {
	const std::string id = "Transform##id" + std::to_string(id_);
	if (ImGui::CollapsingHeader(id.c_str())) {
		if (ImGui::TreeNodeEx("scale", ImGuiTreeNodeFlags_DefaultOpen)) {
			std::string scaleName = "scale##" + id;
			ImGui::DragFloat3(scaleName.c_str(), &srt_.scale.x, 0.01f);
			ImGui::TreePop();
		}

		if (ImGui::TreeNodeEx("rotate", ImGuiTreeNodeFlags_DefaultOpen)) {
			std::string rotateName = "rotate##" + id;
			ImGui::DragFloat3("rotate", &srt_.rotate.x, 0.1f);
			ImGui::DragFloat3(rotateName.c_str(), &moveQuaternion_.x, 0.001f);

			ImGui::TreePop();
		}

		if (ImGui::TreeNodeEx("translation", ImGuiTreeNodeFlags_DefaultOpen)) {
			std::string translateName = "translate##" + id;
			ImGui::DragFloat3(translateName.c_str(), &srt_.translate.x, 0.1f);
			ImGui::TreePop();
		}
	}
}

void AOENGINE::WorldTransform::Manipulate(const ImVec2& windowSize, const ImVec2& imagePos) {
	ImGuizmo::PushID(id_);
	ImGuizmo::SetDrawlist(ImGui::GetWindowDrawList()); // ←画面全体描画リスト
	ImGuizmo::SetRect(imagePos.x, imagePos.y, windowSize.x, windowSize.y);

	Math::Matrix4x4 viewMat = AOENGINE::Render::GetViewport3D();
	Math::Matrix4x4 projectMat = Math::Matrix4x4::MakePerspectiveFov(0.45f, float(windowSize.x) / float(windowSize.y), 0.1f, 100.0f);

	float view[16];
	float proj[16];
	float world[16];

	memcpy(view, &viewMat, sizeof(view));
	memcpy(proj, &projectMat, sizeof(proj));
	memcpy(world, &worldMat_, sizeof(world));

	if (ManipulateTool::type_ == UseManipulate::Scale) {
		ImGuizmo::Manipulate(view, proj, ImGuizmo::SCALE, ImGuizmo::LOCAL, world);
	}

	if (ManipulateTool::type_ == UseManipulate::Rotate) {
		ImGuizmo::Manipulate(view, proj, ImGuizmo::ROTATE, ImGuizmo::LOCAL, world);
	}

	if (ManipulateTool::type_ == UseManipulate::Translate) {
		ImGuizmo::Manipulate(view, proj, ImGuizmo::TRANSLATE, ImGuizmo::LOCAL, world);
	}

	if (ImGuizmo::IsUsing()) {
		memcpy(&worldMat_, world, sizeof(world));
		srt_.scale = worldMat_.GetScale();
		srt_.rotate = worldMat_.GetRotate();
		srt_.translate = worldMat_.GetPosition();
	}

	ImGuizmo::PopID();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　Setter系
//////////////////////////////////////////////////////////////////////////////////////////////////

void AOENGINE::WorldTransform::SetParent(const Math::Matrix4x4& parentMat) {
	parentWorldMat_ = &parentMat;
}

void AOENGINE::WorldTransform::SetMatrix(const Math::Matrix4x4& mat) {
	data_->matWorld = mat;
	data_->worldInverseTranspose = Inverse(data_->matWorld).Transpose();
}
