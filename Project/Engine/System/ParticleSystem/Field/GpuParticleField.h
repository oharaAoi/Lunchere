#pragma once
#include "Engine/Lib/Math/Vector3.h"
#include "Engine/DirectX/Utilities/DirectXUtils.h"
#include "Engine/Module/Components/Attribute/AttributeGui.h"

namespace AOENGINE {

/// <summary>
/// GpuParticleが影響を受けるFieldクラス
/// </summary>
class GpuParticleField :
	public AOENGINE::AttributeGui {
public:

	/// <summary>
	/// GPUパーティクル加速フィールド情報
	/// </summary>
	struct AccelerationField {
		Math::Vector3 acceleration;
		float pad1;
		Math::Vector3 min;
		float pad2;
		Math::Vector3 max;
		float pad3;
	};

	/// <summary>
	/// フレームごとのフィールド情報
	/// </summary>
	struct PerFrame {
		float time;
		float deltaTime;
	};

public:

	GpuParticleField() = default;
	~GpuParticleField();

public:

	// 編集処理
	void Debug_Gui() override;
	// 初期化処理
	void Init(uint32_t _instanceNum);
	// 更新処理
	void Update();
	// 領域の描画
	void DrawShape() const;
	// 実行
	void Execute(ID3D12GraphicsCommandList* commandList);

public:

	void SetParticleResourceHandle(const D3D12_GPU_DESCRIPTOR_HANDLE& _handle) { particleResourceHandle_ = _handle; }
	void SetMaxParticleResource(ID3D12Resource* _resource) { maxParticleResource_ = _resource; }

private:

	bool isEnable_;

	float timer_;

	// resource
	D3D12_GPU_DESCRIPTOR_HANDLE particleResourceHandle_;

	ComPtr<ID3D12Resource> fieldBuffer_;
	AccelerationField* fieldData_;

	ComPtr<ID3D12Resource> perFrameBuffer_;
	PerFrame* perFrame_;

	ID3D12Resource* maxParticleResource_;

	uint32_t kInstanceNum_;
};

}