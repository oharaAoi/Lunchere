#pragma once
// DirectX
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>
#include <string>

namespace AOENGINE {

/// <summary>
/// Blend設定クラス
/// </summary>
class Blend {
public:

	enum BlendMode {
		// ブレンドなし
		None,
		// 通常αブレンド
		Normal,
		// 加算
		Add,
		// 減算
		Subtract,
		// 乗算
		Multiply,
		// 反転
		Screen
	};

public:

	Blend();
	~Blend();

	void Init();

	D3D12_BLEND_DESC SetBlend(const BlendMode& mode);

	D3D12_BLEND_DESC SetBlend(const std::string& modeName);

	void SetNormalBlend(D3D12_BLEND_DESC& blendDesc);

	void SetAddBlend(D3D12_BLEND_DESC& blendDesc);

	void SetSubtractBlend(D3D12_BLEND_DESC& blendDesc);

	void SetMultiplyBlend(D3D12_BLEND_DESC& blendDesc);

	void SetScreenBlend(D3D12_BLEND_DESC& blendDesc);

};

}