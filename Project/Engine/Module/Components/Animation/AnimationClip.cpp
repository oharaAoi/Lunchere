#include "AnimationClip.h"
#include "Engine/Module/Components/Animation/AnimationFunctions.h"
#include "Engine/System/Manager/AnimationManager.h"
#include "Engine/System/Manager/ImGuiManager.h"
#include "Engine/Lib/Math/Quaternion.h"
#include "Engine/Lib/GameTimer.h"

using namespace AOENGINE;

AnimationClip::AnimationClip() {}
AnimationClip ::~AnimationClip() {}

//==================================================================================
// ↓　初期化処理
//==================================================================================

void AnimationClip::Init(const std::string& rootName, bool isSkinning, bool isLoop) {
	manager_ = AnimationManager::GetInstance();

	rootName_ = rootName;
	isSkinnig_ = isSkinning;
	isLoop_ = isLoop;

	isAnimationChange_ = false;
	blendFactor_ = 0.0f;
	blendSpeed_ = 1.0f;

	lerpAnimationNamesIndex_[0] = 0;
	lerpAnimationNamesIndex_[1] = 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　更新処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void AnimationClip::Update() {
	nowAnimationName_ = animation_.animationName;
	if (isStop_) {
		return;
	}

	// アニメーションが終了したら
	if (isAnimationFinish_) {
		if (isLoop_) {
			isAnimationFinish_ = false;
			animationTime_ = std::fmod(animationTime_, animation_.duration);
		}
	}

	// animationを進める
	animationTime_ += AOENGINE::GameTimer::DeltaTime() * animationSpeed_;
	if (animationTime_ >= animation_.duration) {
		isAnimationFinish_ = true;
	}

	// 遷移の予約が入っていたら
	if (isReservation_) {
		if ((animationTime_ / animation_.duration) >= startTransitionRaito_) {
			isAnimationChange_ = true;
			isReservation_ = false;
			lerpAnimationTime_[0] = animationTime_;
			lerpAnimationTime_[1] = 0;
			return;
		}
	}

	// skinningを行わない場合アニメーションの行列を更新する
	if (!isSkinnig_) {
		NodeAnimation& rootNodeAnimation = animation_.nodeAnimations[rootName_];
		Math::Vector3 translate = CalculateValue(rootNodeAnimation.translate.keyframes, animationTime_);
		Math::Quaternion rotate = CalculateQuaternion(rootNodeAnimation.rotate.keyframes, animationTime_);
		Math::Vector3 scale = CalculateValue(rootNodeAnimation.scale.keyframes, animationTime_);
		animationMat_ = Math::Matrix4x4::MakeAffine(scale, rotate, translate);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　読み込み
//////////////////////////////////////////////////////////////////////////////////////////////////

void AnimationClip::LoadAnimation(const std::string directoryPath, const std::string& animationFile) {
	animationFileName_ = animationFile;
	// animationが登録されているかを確認
	if (manager_->CheckAnimationMap(animationFile)) {
		nowAnimationName_ = manager_->GetAnimationFirstName(animationFile);
		animation_ = manager_->GetAnimation(animationFile, nowAnimationName_);
	} else {
		animation_ = manager_->LoadAnimationFile(directoryPath, animationFile);
		nowAnimationName_ = manager_->GetAnimationFirstName(animationFile);
	}

	animationNames_ = manager_->GetModelHaveAnimationNames(animationFile);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　animationの適応
//////////////////////////////////////////////////////////////////////////////////////////////////

void AnimationClip::ApplyAnimation(Skeleton* skeleton) {
	for (Skeleton::Joint& joint : skeleton->GetJoints()) {
		if (auto it = animation_.nodeAnimations.find(joint.name); it != animation_.nodeAnimations.end()) {
			const NodeAnimation& rootNodeAnimation = (*it).second;
			joint.transform.translate = CalculateValue(rootNodeAnimation.translate.keyframes, animationTime_);
			joint.transform.rotate = CalculateQuaternion(rootNodeAnimation.rotate.keyframes, animationTime_);
			joint.transform.scale = CalculateValue(rootNodeAnimation.scale.keyframes, animationTime_);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　animationを遷移させる
//////////////////////////////////////////////////////////////////////////////////////////////////

void AnimationClip::LerpApplyAnimation(Skeleton* skeleton) {
	blendFactor_ += AOENGINE::GameTimer::DeltaTime() * animationSpeed_;

	if (lerpAnimationTime_[0] < lerpAnimetion_[0].duration) {
		if (!poseToAnimation_) {
			lerpAnimationTime_[0] += AOENGINE::GameTimer::DeltaTime() * animationSpeed_;
		}
	}

	if (lerpAnimationTime_[1] < lerpAnimetion_[1].duration) {
		lerpAnimationTime_[1] += AOENGINE::GameTimer::DeltaTime() * animationSpeed_;

	}

	for (Skeleton::Joint& joint : skeleton->GetJoints()) {
		auto itA = lerpAnimetion_[0].nodeAnimations.find(joint.name);
		// アニメーションBのノードアニメーションを取得
		auto itB = lerpAnimetion_[1].nodeAnimations.find(joint.name);

		if (itA != lerpAnimetion_[0].nodeAnimations.end() && itB != lerpAnimetion_[1].nodeAnimations.end()) {
			const NodeAnimation& nodeAnimationA = itA->second;
			const NodeAnimation& nodeAnimationB = itB->second;

			// アニメーションAとBのそれぞれの変換を取得
			Math::Vector3 translateA = CalculateValue(nodeAnimationA.translate.keyframes, lerpAnimationTime_[0]);
			Math::Quaternion rotateA = CalculateQuaternion(nodeAnimationA.rotate.keyframes, lerpAnimationTime_[0]);
			Math::Vector3 scaleA = CalculateValue(nodeAnimationA.scale.keyframes, lerpAnimationTime_[0]);

			Math::Vector3 translateB = CalculateValue(nodeAnimationB.translate.keyframes, lerpAnimationTime_[1]);
			Math::Quaternion rotateB = CalculateQuaternion(nodeAnimationB.rotate.keyframes, lerpAnimationTime_[1]);
			Math::Vector3 scaleB = CalculateValue(nodeAnimationB.scale.keyframes, lerpAnimationTime_[1]);

			// blendFactor（補間率）に基づいて補間
			float t = blendFactor_ / blendSpeed_;
			joint.transform.translate = Lerp(translateA, translateB, t);
			joint.transform.rotate = Math::Quaternion::Slerp(rotateA, rotateB, t);
			joint.transform.scale = Lerp(scaleA, scaleB, t);
		}
	}

	// 1以上になったら完全に遷移させる
	if (blendFactor_ >= blendSpeed_) {
		isAnimationChange_ = false;
		animationTime_ = blendFactor_ / lerpAnimetion_[1].duration;
		animation_ = lerpAnimetion_[1];
		poseToAnimation_ = false;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　animation同士の補完を設定する
//////////////////////////////////////////////////////////////////////////////////////////////////

void AnimationClip::LerpAnimation(const std::string& preAnimation, const std::string& lerpAnimation, float blendSpeed) {
	lerpAnimetion_[0] = manager_->GetAnimation(animationFileName_, preAnimation);
	lerpAnimetion_[1] = manager_->GetAnimation(animationFileName_, lerpAnimation);

	lerpAnimationTime_[0] = animationTime_;
	lerpAnimationTime_[1] = 0;

	isAnimationChange_ = true;

	blendFactor_ = 0.0f;

	nowAnimationName_ = lerpAnimation;
	blendSpeed_ = blendSpeed;
}

void AnimationClip::PoseToAnimation(const std::string& afterAnimationName, float lerpTime) {
	lerpAnimetion_[0] = animation_;
	lerpAnimetion_[1] = manager_->GetAnimation(animationFileName_, afterAnimationName);

	lerpAnimationTime_[0] = animationTime_;
	lerpAnimationTime_[1] = 0;

	isAnimationChange_ = true;
	poseToAnimation_ = true;

	blendFactor_ = 0.0f;
	nowAnimationName_ = afterAnimationName;
	blendSpeed_ = lerpTime;
}


void AnimationClip::LerpAnimation(const std::string& lerpAnimation, float blendSpeed) {
	// 今と遷移後がAnimationが同じだったら何もしない
	if (animation_.animationName == lerpAnimation) {
		return;
	}

	lerpAnimetion_[0] = animation_;
	lerpAnimetion_[1] = manager_->GetAnimation(animationFileName_, lerpAnimation);

	lerpAnimationTime_[0] = animationTime_;
	lerpAnimationTime_[1] = 0;

	isAnimationChange_ = true;

	blendFactor_ = 0.0f;
	blendSpeed_ = blendSpeed;

	nowAnimationName_ = lerpAnimation;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　Animationを再設定する
//////////////////////////////////////////////////////////////////////////////////////////////////

void AnimationClip::ResetAnimation(const std::string& animationName) {
	animation_ = manager_->GetAnimation(animationFileName_, animationName);
	nowAnimationName_ = animation_.animationName;
	animationTime_ = 0.0f;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　Animationの予約を行う
//////////////////////////////////////////////////////////////////////////////////////////////////

void AnimationClip::ReservationAnimation(const std::string& preAnimation, const std::string& lerpAnimation, float blendSpeed, float startTransitionRaito) {
	lerpAnimetion_[0] = manager_->GetAnimation(animationFileName_, preAnimation);
	lerpAnimetion_[1] = manager_->GetAnimation(animationFileName_, lerpAnimation);

	lerpAnimationTime_[0] = animationTime_;
	lerpAnimationTime_[1] = 0;

	isReservation_ = true;
	startTransitionRaito_ = startTransitionRaito;
	blendSpeed_ = blendSpeed;
	blendFactor_ = 0.0f;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　Debug
//////////////////////////////////////////////////////////////////////////////////////////////////

void AnimationClip::Debug_Gui() {
	bool isChange = false;
	int isAnimationFinish = (int)isAnimationFinish_;

	if (ImGui::Button("play")) {
		isStop_ = false;
	}
	ImGui::SameLine();
	if (ImGui::Button("stop")) {
		isStop_ = true;
	}
	ImGui::SameLine();
	if (ImGui::Button("reset")) {
		animationTime_ = 0.0f;
	}

	ImGui::Checkbox("isLoop", &isLoop_);

	ImGui::SliderFloat("time", &animationTime_, 0.0f, animation_.duration);
	ImGui::DragFloat("speed", &animationSpeed_, 0.1f);
	ImGui::SliderInt("isFinish", &isAnimationFinish, 0, 1);
	ImGui::Text(nowAnimationName_.c_str());

	{
		ImGui::BulletText("SelectAnimation");
		// 現在のanimationを設定する
		if (ImGui::BeginCombo("## select", animationNames_[selectedAnimationIndex].c_str())) {
			for (int i = 0; i < animationNames_.size(); ++i) {
				bool isSelected = (i == selectedAnimationIndex);
				if (ImGui::Selectable(animationNames_[i].c_str(), isSelected)) {
					selectedAnimationIndex = i; // インデックスを更新
					isChange = true;
				}
				if (isSelected) {
					ImGui::SetItemDefaultFocus(); // 初期選択のフォーカス
				}
			}
			ImGui::EndCombo();
		}

		if (isChange) {
			std::string animationName = animationNames_[selectedAnimationIndex];
			animation_ = manager_->GetAnimation(animationFileName_, animationName);
		}
	}

	{

		// 遷移後アニメーション
		if (ImGui::BeginCombo("after", animationNames_[lerpAnimationNamesIndex_[1]].c_str())) {
			for (int i = 0; i < animationNames_.size(); ++i) {
				bool isSelected = (i == lerpAnimationNamesIndex_[1]);
				if (ImGui::Selectable(animationNames_[i].c_str(), isSelected)) {
					lerpAnimationNamesIndex_[1] = i; // インデックスを更新
				}
				if (isSelected) {
					ImGui::SetItemDefaultFocus(); // 初期選択のフォーカス
				}
			}
			ImGui::EndCombo();
		}

		ImGui::DragFloat("blendSpeed", &blendSpeed_, 0.01f);

		if (ImGui::Button("isChange")) {
			LerpAnimation(
				animationNames_[lerpAnimationNamesIndex_[0]],
				animationNames_[lerpAnimationNamesIndex_[1]],
				blendSpeed_
			);
		}

		ImGui::BulletText("Transition Animation");
		ImGui::SliderFloat("lerpAnimationTime_0", &lerpAnimationTime_[0], 0.0f, lerpAnimetion_[0].duration);
		ImGui::SliderFloat("lerpAnimationTime_1", &lerpAnimationTime_[1], 0.0f, lerpAnimetion_[1].duration);
		// 遷移前アニメーション
		if (ImGui::BeginCombo("before", animationNames_[lerpAnimationNamesIndex_[0]].c_str())) {
			for (int i = 0; i < animationNames_.size(); ++i) {
				bool isSelected = (i == lerpAnimationNamesIndex_[0]);
				if (ImGui::Selectable(animationNames_[i].c_str(), isSelected)) {
					lerpAnimationNamesIndex_[0] = i; // インデックスを更新
				}
				if (isSelected) {
					ImGui::SetItemDefaultFocus(); // 初期選択のフォーカス
				}
			}
			ImGui::EndCombo();
		}
	}

	if (isAnimationChange_) {
		ImGui::Text("true Change");
	} else {
		ImGui::Text("not Change");
	}

	ImGui::Text(nowAnimationName_.c_str());
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　ImGuiで選択されているAnimatinoの名前を返す
//////////////////////////////////////////////////////////////////////////////////////////////////

std::string AnimationClip::SelectAnimationName() {
	if (ImGui::BeginCombo("Select Animation", animationNames_[selectedAnimationIndex].c_str())) {
		for (int i = 0; i < animationNames_.size(); ++i) {
			bool isSelected = (i == selectedAnimationIndex);
			if (ImGui::Selectable(animationNames_[i].c_str(), isSelected)) {
				selectedAnimationIndex = i; // インデックスを更新
			}
			if (isSelected) {
				ImGui::SetItemDefaultFocus(); // 初期選択のフォーカス
			}
		}
		ImGui::EndCombo();
	}

	return animationNames_[selectedAnimationIndex];
}
