#pragma once
#include <string>
#include "Engine/Lib/Math/Vector3.h"
#include "Engine/DirectX/Utilities/DirectXUtils.h"
#include "Engine/Lib/GpuParticlesData.h"
#include "Engine/Module/Components/Attribute/AttributeGui.h"

namespace AOENGINE {

/// <summary>
/// gpuParticleを射出するemitter
/// </summary>
class GpuParticleEmitter :
	public AOENGINE::AttributeGui {
public:

	/// <summary>
	/// フレームごとのエミッター情報
	/// </summary>
	struct PerFrame {
		float time;
		float deltaTime;
	};

public:

	GpuParticleEmitter() = default;
	~GpuParticleEmitter();

public:

	// 編集処理
	void Debug_Gui() override;

	// 初期化処理
	void Init(const std::string& name);

	// 更新処理
	void Update();

	// コマンドのバインドする
	void EmitCommand(ID3D12GraphicsCommandList* commandList);

	// 形状の描画
	void DrawShape(const Math::Matrix4x4& viewProjectionMatrix) const;

public:

	void SetParticleResourceHandle(const D3D12_GPU_DESCRIPTOR_HANDLE& _handle) { particleResourceHandle_ = _handle; }
	void SetFreeListIndexHandle(const D3D12_GPU_DESCRIPTOR_HANDLE& _handle) { freeListIndexHandle_ = _handle; }
	void SetFreeListHandle(const D3D12_GPU_DESCRIPTOR_HANDLE& _handle) { freeListHandle_ = _handle; }
	void SetMaxParticleResource(ID3D12Resource* _resource) { maxParticleResource_ = _resource; }

public:

	void SetIsStop(bool _isStop) { isStop_ = _isStop; }

	void SetParent(const Math::Matrix4x4& parentMat);

	void SetLocalPos(const Math::Vector3& _pos) { emitterItem_.pos = _pos; }

protected:

	void SetItem();

protected:

	ID3D12Device* dxDevice_;
	ID3D12GraphicsCommandList* commandList_;

	D3D12_GPU_DESCRIPTOR_HANDLE particleResourceHandle_;
	D3D12_GPU_DESCRIPTOR_HANDLE freeListIndexHandle_;
	D3D12_GPU_DESCRIPTOR_HANDLE freeListHandle_;

	ComPtr<ID3D12Resource> emitterResource_;
	AOENGINE::GpuParticleEmitterData* emitterData_;

	ComPtr<ID3D12Resource> perFrameBuffer_;
	PerFrame* perFrame_;

	ID3D12Resource* maxParticleResource_;

	AOENGINE::GpuParticleEmitterItem emitterItem_;

	// parameter
	bool isStop_ = false;
	float emitAccumulator_;
	float currentTimer_;

	int emitCount_;

	std::string emitCsName_;

	// 親のMatrix
	const Math::Matrix4x4* parentWorldMat_ = nullptr;

};

}