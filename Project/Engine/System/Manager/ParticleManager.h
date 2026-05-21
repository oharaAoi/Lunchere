#pragma once
#include <string>
#include <memory>
#include "Engine/Lib/ParticlesData.h"
#include "Engine/Render/ParticleInstancingRenderer.h"
#include "Engine/Module/Components/Effect/BaseParticles.h"
#include "Engine/Module/Components/Attribute/AttributeGui.h"
#include "Engine/System/ParticleSystem/Particle/CpuParticleUpdater.h"

namespace AOENGINE {

/// <summary>
/// パーティクル管理クラス
/// </summary>
class ParticleManager :
	public AOENGINE::AttributeGui {
public:

	/// <summary>
	/// パーティクルデータ情報
	/// </summary>
	struct ParticlesData {
		std::shared_ptr<std::list<AOENGINE::ParticleSingle>> particles;
		std::vector<AOENGINE::ParticleInstancingRenderer::ParticleData> forGpuData_;
		bool isAddBlend = false;
		bool anyParticleAlive = false;

		ParticlesData() {
			particles = std::make_shared<std::list<AOENGINE::ParticleSingle>>();
		}
	};

public:

	ParticleManager() = default;
	~ParticleManager();
	ParticleManager(const ParticleManager&) = delete;
	const ParticleManager& operator=(const ParticleManager&) = delete;

	static AOENGINE::ParticleManager* GetInstance();

public:

	// 終了
	void Finalize();
	// 初期化
	void Init();
	// 更新
	void Update();
	// particleの更新
	void ParticlesUpdate();
	// 全体更新後の更新
	void PostUpdate();
	// 描画処理
	void Draw() const;
	// 編集処理
	void Debug_Gui() override;

public:

	/// <summary>
	/// View系のMatrixを設定する
	/// </summary>
	/// <param name="view">: viewMatrix</param>
	/// <param name="bill">: billBordMatrix</param>
	void SetView(const Math::Matrix4x4& view, const Math::Matrix4x4& viewProj2d, const Math::Matrix4x4& bill) {
		particleRenderer_->SetView(view, viewProj2d, bill);
	}

	//void AddParticleList(AOENGINE::BaseParticles* particles);

	/// <summary>
	/// Particleを作成する
	/// </summary>
	/// <param name="particlesFile"></param>
	/// <returns></returns>
	AOENGINE::BaseParticles* CreateParticle(const std::string& particlesFile);

	void DeleteParticles(AOENGINE::BaseParticles* ptr);

private:

	// particleの描画を呼び出すレンダラー
	std::unique_ptr<AOENGINE::ParticleInstancingRenderer> particleRenderer_;
	// particleを射出するリスト
	std::list<std::unique_ptr<AOENGINE::BaseParticles>> emitterList_;

	CpuParticleUpdater particleUpdater_;

};

}