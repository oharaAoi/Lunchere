#pragma once
#include <d3dx12.h>

/// <summary>
/// 2D ComputeShaderのDispatchサイズ
/// </summary>
struct ComputeDispatchSize2D {
	UINT x;
	UINT y;

	ComputeDispatchSize2D(UINT _x, UINT _y) : x(_x), y(_y) {};
};

ComputeDispatchSize2D CalcDispatchSize2D(UINT _width, UINT _height, UINT _threadX = 16, UINT _threadY = 16);

ComputeDispatchSize2D CalcDispatchSize(const D3D12_RESOURCE_DESC& desc, UINT _threadX = 16, UINT _threadY = 16);