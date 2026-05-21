#pragma once
#include <string>
#include <memory>
#include "Engine/DirectX/Utilities/DirectXUtils.h"
#include "Engine/DirectX/Resource/DxResource.h"
#include "Engine/DirectX/Resource/DxResourceManager.h"
#include "Engine/Module/Components/GameObject/GeometryObject.h"

namespace AOENGINE {

/// <summary>
/// gpuParticleを描画する
/// </summary>
class GpuParticleRenderer {
public:	// メンバ構造体

	/// <summary>
	/// 描画用GPUパーティクル情報
	/// </summary>
	struct Particle {
		Math::Vector4 color;			// 色
		Math::Vector3 scale;			// 拡縮
		Math::Vector3 targetScale;
		Math::Vector3 rotate;			// 回転
		Math::Vector3 translate;		// 座標
		Math::Vector3 velocity;		// 速度
		Math::Vector3 acceleration;
		// 加速度
		float lifeTime;			// 生存時間
		float currentTime;		// 現在の時間
		float damping;			// 減衰
		float gravity;			// 重力

		int emitType;
		int lifeOfScaleDown;
		int lifeOfScaleUp;
		int lifeOfAlpha;

		int isDraw2d;
		int beAffectedByField = 0;
	};

	/// <summary>
	/// ビューごとのGPUパーティクル描画情報
	/// </summary>
	struct PerView {
		Math::Matrix4x4 viewProjection;
		Math::Matrix4x4 billboardMat;
	};

	/// <summary>
	/// フレームごとのGPUパーティクル描画情報
	/// </summary>
	struct PerFrame {
		float time;
		float deltaTime;
	};

	/// <summary>
	/// 最大パーティクル数情報
	/// </summary>
	struct MaxParticles {
		int count;
	};

public:

	GpuParticleRenderer() = default;
	~GpuParticleRenderer();

public:

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="_instanceNum">: インスタンス数</param>
	void Init(uint32_t _instanceNum);

	// 更新
	void Update();

	// 描画
	void Draw() const;

private:

	/// <summary>
	/// resourceの作成
	/// </summary>
	/// <param name="_dxHeap"></param>
	/// <param name="_dxDevice"></param>
	void CreateResource(AOENGINE::DxResourceManager* _resourceManager);

public:

	D3D12_GPU_DESCRIPTOR_HANDLE GetResourceHandle() const { return particleResource_->GetUAV().handleGPU; }
	D3D12_GPU_DESCRIPTOR_HANDLE GetFreeListIndexHandle() const { return freeListIndexResource_->GetUAV().handleGPU; }
	D3D12_GPU_DESCRIPTOR_HANDLE GetFreeListHandle() const { return freeListResource_->GetUAV().handleGPU; }
	ID3D12Resource* GetMaxBufferResource() const { return maxParticleBuffer_.Get(); }

	void SetView(const Math::Matrix4x4& view, const Math::Matrix4x4& bill) {
		perView_->viewProjection = view;
		perView_->billboardMat = Multiply(Math::Quaternion::AngleAxis(kPI, CVector3::UP).MakeMatrix(), bill);
	}

private:

	uint32_t kInstanceNum_;

	AOENGINE::DxResource* particleResource_;
	AOENGINE::DxResource* freeListIndexResource_;
	AOENGINE::DxResource* freeListResource_;

	ComPtr<ID3D12Resource> perViewBuffer_;
	PerView* perView_;

	ComPtr<ID3D12Resource> perFrameBuffer_;
	PerFrame* perFrame_;

	ComPtr<ID3D12Resource> maxParticleBuffer_;
	MaxParticles* maxBuffer_;

	// meshの形状
	std::unique_ptr<AOENGINE::GeometryObject> shape_;
	std::unique_ptr<AOENGINE::Material> material_;

	uint32_t groups_;
	const uint32_t threads_ = 256;

};

}