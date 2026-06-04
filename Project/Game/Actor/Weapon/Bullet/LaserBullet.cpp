#include "LaserBullet.h"
#include "Engine/Core/Engine.h"
#include "Engine/Render/SceneRenderer.h"
#include "Engine/System/Manager/ParticleManager.h"
#include "Engine/System/Audio/AudioPlayer.h"
#include "Engine/Lib/GameTimer.h"
#include "Game/Information/ColliderCategory.h"

LaserBullet::~LaserBullet() {
	BaseBullet::Finalize();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 編集処理
///////////////////////////////////////////////////////////////////////////////////////////////

void LaserBullet::Debug_Gui() {
	isFade_ = false;

	param_.Debug_Gui();
	parentTransform_->Debug_Gui();
	BaseBullet::Debug_Gui();
	ImGui::DragFloat3("targetPos", &targetPos_.x);

	param_.shaderGraphPath = laserCylinder_->GetShaderGraphPath();
	if (ImGui::Button("shot")) {
		isShot_ = true;
		parentTransform_->SetScale(Math::Vector3(1, 1, 0.0f));
		Reset(CVector3::ZERO, targetPos_, 200.0f);
	}
}

void LaserBullet::LaserParameter::Debug_Gui() {
	ImGui::DragFloat("maxLength", &maxLength);
	ImGui::DragFloat("fadeTime", &fadeTime);
	ImGui::DragFloat("lifeTime", &lifeTime);
	ImGui::DragFloat("shotSeValue", &shotSeValue);
	ImGui::DragFloat("uvScaleY", &uvScaleY);
	ImGui::ColorEdit4("color", &cylinderColor.r);
	ImGui::Text("shaderGraphPath : %s", shaderGraphPath.c_str());
	SaveAndLoad();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 初期化処理
///////////////////////////////////////////////////////////////////////////////////////////////

void LaserBullet::Init() {
	BaseBullet::Init("LaserBullet");
	type_ = BulletType::Laser;
	param_.Load();

	// ----------------------
	// ↓ objectの設定
	// ----------------------
	object_->SetObject("laser.obj");
	object_->SetTexture("laser.png");
	object_->SetIsLighting(false);
	object_->SetEnableShadow(false);

	// ----------------------
	// ↓ colliderの設定
	// ----------------------
	AOENGINE::BaseCollider* collider = object_->SetCollider(ColliderTags::Bullet::machinegun, ColliderShape::Line);
	collider->SetTarget(ColliderTags::Boss::own);
	collider->SetTarget(ColliderTags::Enemy::own);
	collider->SetTarget(ColliderTags::Field::ground);
	collider->SetTarget(ColliderTags::None::own);
	collider->SetIsTrigger(true);
	collider->SetOnCollision([this](AOENGINE::BaseCollider* other) { OnCollision(other); });
	lineCollider_ = dynamic_cast<AOENGINE::LineCollider*>(collider);

	// ----------------------
	// ↓ Parameter系の設定
	// ----------------------
	parentTransform_ = std::make_unique<AOENGINE::WorldTransform>();
	parentTransform_->Init();

	transform_->SetTranslationZ(1.0f);
	transform_->SetParent(parentTransform_->GetWorldMatrix());

	isShot_ = true;
	isFade_ = false;

	// ----------------------
	// ↓ Effectの設定
	// ----------------------
	laserCylinder_ = std::make_unique<LaserCylinder>();
	laserCylinder_->Init(param_.cylinderColor);
	laserCylinder_->GetShaderGraph()->Load(param_.shaderGraphPath);
	laserCylinder_->GetTransform()->SetParent(parentTransform_->GetWorldMatrix());
	AddChild(laserCylinder_.get());

	AOENGINE::ParticleManager* manager = AOENGINE::ParticleManager::GetInstance();
	shotEffect_ = manager->CreateParticle("laserShot");
	shotParticle_ = manager->CreateParticle("laserParticle");

	fadeTimer_ = AOENGINE::Timer(param_.fadeTime);
	lifeTimer_ = AOENGINE::Timer(param_.lifeTime);

	AOENGINE::SceneRenderer::GetInstance()->ChangeRenderingType("Object_laser.json", object_);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 更新処理
///////////////////////////////////////////////////////////////////////////////////////////////

void LaserBullet::Update() {
	if (lifeTimer_.Run(AOENGINE::GameTimer::DeltaTime())) {
		float z = std::lerp(1.f, 0.f, lifeTimer_.t_);
		parentTransform_->SetScaleX(z);
		parentTransform_->SetScaleY(z);

		transform_->SetScaleZ(z);
		laserCylinder_->SetScaleZ(z);
	} else {
		isAlive_ = false;
	}

	Stretch();

	Fade();

	laserCylinder_->Update();
	lineCollider_->Update(Math::QuaternionSRT(CVector3::ZERO, Math::Quaternion(), parentTransform_->GetTranslate()));
	parentTransform_->Update();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ リセット処理
///////////////////////////////////////////////////////////////////////////////////////////////

void LaserBullet::Reset(const Math::Vector3& _pos, const Math::Vector3& _targetPos, float _speed) {
	parentTransform_->SetTranslate(_pos);
	dire_ = Math::Vector3(_targetPos - _pos).Normalize();
	parentTransform_->SetRotate(Math::Quaternion::LookRotation(dire_));
	speed_ = _speed;
	shotParticle_->SetPos(_pos);
	shotEffect_->SetPos(_pos);
	shotParticle_->Reset();
	shotEffect_->Reset();

	isShot_ = true;
	isFade_ = false;

	fadeTimer_ = AOENGINE::Timer(param_.fadeTime);

	// 音を鳴らす
	AOENGINE::AudioPlayer::SingleShotPlay("laser.mp3", param_.shotSeValue);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 伸ばす
///////////////////////////////////////////////////////////////////////////////////////////////

void LaserBullet::Stretch() {
	// scaleを大きくする
	if (isShot_) {
		Math::Vector3 scale = parentTransform_->GetScale();
		scale.z += speed_ * AOENGINE::GameTimer::DeltaTime();
		parentTransform_->SetScale(scale);

		if (scale.z >= param_.maxLength) {
			isShot_ = false;
			isFade_ = true;
		}

		Math::Vector3 diff = dire_ * scale.z;
		lineCollider_->SetDiff(diff);
		laserCylinder_->SetUvScale(param_.uvScaleY * scale.z);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 消える
///////////////////////////////////////////////////////////////////////////////////////////////

void LaserBullet::Fade() {
	if (isFade_) {
		if (fadeTimer_.Run(AOENGINE::GameTimer::DeltaTime())) {
			float z = std::lerp(1.f, 0.f, fadeTimer_.t_);
			parentTransform_->SetScaleX(z);
			parentTransform_->SetScaleY(z);

			transform_->SetScaleZ(z);
			laserCylinder_->SetScaleZ(z);

			object_->SetColor(AOENGINE::Color(1, 1, 1, z));
			laserCylinder_->GetGameObject()->SetColor(AOENGINE::Color(1, 1, 1, z));

		} else {
			isAlive_ = false;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// ↓ 衝突処理
///////////////////////////////////////////////////////////////////////////////////////////////

void LaserBullet::OnCollision(AOENGINE::BaseCollider* _other) {
	if (_other->GetCategoryName() == ColliderTags::None::own || _other->GetCategoryName() == ColliderTags::Boss::own) {
		isFade_ = true;

		AOENGINE::ParticleManager* manager = AOENGINE::ParticleManager::GetInstance();
		AOENGINE::BaseParticles* hitLaserEffect_ = manager->CreateParticle("LaserHitSpark");
		AOENGINE::BaseParticles* hitLaserParticle_ = manager->CreateParticle("LaserHitParticle");

		Math::Vector3 scale = parentTransform_->GetScale();
		Math::Vector3 hitPos = lineCollider_->GetOrigine() + lineCollider_->GetDiff();

		hitLaserEffect_->SetPos(hitPos);
		hitLaserParticle_->SetPos(hitPos);
		hitLaserEffect_->Reset();
		hitLaserParticle_->Reset();

		lineCollider_->SetIsActive(false);
	}
}