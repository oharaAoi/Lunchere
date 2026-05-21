#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <vector>
#include <string>
// Engine
#include "Engine/DirectX/DirectXCompiler/DirectXCompiler.h"
#include "Engine/DirectX/DirectXDevice/DirectXDevice.h"

namespace AOENGINE {

/// <summary>
/// Pipeline生成クラス
/// </summary>
class PipelineFactory {
public:

	PipelineFactory() = default;
	~PipelineFactory() = default;

	void Init(DirectXDevice* _dxDevice, DirectXCompiler* _dxCompiler);

	void ShaderCompile();

	std::vector<D3D12_INPUT_ELEMENT_DESC> CreateInputLayout();

	DXGI_FORMAT ReturnFormat(LPCSTR _name);

	ComPtr<ID3D12RootSignature> CreateRootSignature();

private:

	DirectXDevice* dxDevice_;
	DirectXCompiler* dxCompiler_;

	ComPtr<ID3D12ShaderReflection> vsReflection_;
	ComPtr<ID3D12ShaderReflection> psReflection_;

	// Shader
	ComPtr<IDxcBlob> vertexShaderBlob_ = nullptr;
	ComPtr<IDxcBlob> pixelShaderBlob_ = nullptr;

	std::string vsName_;
	std::string psName_;

};

}