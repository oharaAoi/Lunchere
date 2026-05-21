#pragma once
#include <string>
#include <unordered_map>
#include "Engine/Module/Components/Meshes/Mesh.h"
#include "Engine/Module/Components/Materials/Material.h"
#include "Engine/Module/Components/GameObject/Model.h"
#include "Engine/Lib/Math/Vector3.h"
#include "Engine/Lib/Math/Vector4.h"
#include "Engine/Lib/Math/Matrix4x4.h"
#include "Engine/DirectX/Utilities/DirectXUtils.h"
#include "Engine/DirectX/Resource/DxResource.h"

namespace AOENGINE {

/// <summary>
/// GpuParticleを行うクラス
/// </summary>
class GpuParticles {
public:	// メンバ構造体

	/// <summary>
	/// GPUパーティクル情報
	/// </summary>
	struct Particle {
		Math::Vector4 color;			// 色
		Math::Vector3 scale;			// 拡縮
		Math::Vector3 rotate;			// 回転
		Math::Vector3 translate;		// 座標
		Math::Vector3 velocity;		// 速度
		Math::Vector3 acceleration;	// 加速度
		float lifeTime;			// 生存時間
		float currentTime;		// 現在の時間
		float damping;			// 減衰
		float gravity;			// 重力

		float pad;
	};

	/// <summary>
	/// ビューごとのGPUパーティクル情報
	/// </summary>
	struct PerView {
		Math::Matrix4x4 viewProjection;
		Math::Matrix4x4 billboardMat;
	};

	/// <summary>
	/// フレームごとのGPUパーティクル情報
	/// </summary>
	struct PerFrame {
		float time;
		float deltaTime;
	};

public:

	GpuParticles();
	~GpuParticles();

public:

	/// <summary>
	/// 終了処理
	/// </summary>
	void Finalize();

	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="instanceNum">: インスタンス数</param>
	void Init(uint32_t instanceNum);

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// コマンドを積む
	/// </summary>
	/// <param name="commandList"></param>
	void Draw(ID3D12GraphicsCommandList* commandList);

	/// <summary>
	/// Particle全体を初期化する
	/// </summary>
	/// <param name="commandList"></param>
	/// <param name="rootParameterIndex"></param>
	void InitBindCmdList(ID3D12GraphicsCommandList* commandList, UINT rootParameterIndex);

	/// <summary>
	/// 射出する際のコマンドを積む
	/// </summary>
	/// <param name="commandList"></param>
	/// <param name="rootParameterIndex"></param>
	void EmitBindCmdList(ID3D12GraphicsCommandList* commandList, UINT rootParameterIndex);

public:

	void SetViewProjection(const Math::Matrix4x4& viewProjection);

private:

	uint32_t kInstanceNum_;

	AOENGINE::DxResource* particleResource_;
	AOENGINE::DxResource* freeListIndexResource_;
	AOENGINE::DxResource* freeListResource_;

	ComPtr<ID3D12Resource> perViewBuffer_;
	PerView* perView_;

	ComPtr<ID3D12Resource> perFrameBuffer_;
	PerFrame* perFrame_;

	std::vector<std::shared_ptr<AOENGINE::Mesh>> meshArray_;
	std::vector<std::unique_ptr<AOENGINE::Material>> materials_;

	std::unordered_map<std::string, ModelMaterialData> materialData_;
};

}