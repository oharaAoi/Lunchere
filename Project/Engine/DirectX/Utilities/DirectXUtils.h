#pragma once
// DirectX
#include <d3d12.h>
#include <cassert>
#include <wrl.h>
#include <dxcapi.h>
#include <string>
#include "Engine/Utilities/Convert.h"

template<typename T>
using ComPtr = Microsoft::WRL::ComPtr <T>;

/// <summary>
/// DescriptorのHandle
/// </summary>
namespace AOENGINE {

struct DescriptorHandles {
	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU;
	D3D12_GPU_DESCRIPTOR_HANDLE handleGPU;
	int assignIndex_;
};

}

using AOENGINE::DescriptorHandles;

/// <summary>
/// uavDescの作成
/// </summary>
/// <param name="numElemnts"></param>
/// <param name="structureByte"></param>
/// <returns></returns>
D3D12_UNORDERED_ACCESS_VIEW_DESC CreateUavDesc(UINT _numElemnts, UINT _structureByte);

/// <summary>
/// srvDescの作成
/// </summary>
/// <param name="numElemnts"></param>
/// <param name="structureByte"></param>
/// <returns></returns>
D3D12_SHADER_RESOURCE_VIEW_DESC CreateSrvDesc(UINT _numElemnts, UINT _structureByte);

/// <summary>
/// UploadResourceDescの作成
/// </summary>
/// <param name="sizeInBytes"></param>
/// <returns></returns>
D3D12_RESOURCE_DESC CreateUploadResourceDesc(size_t _sizeInBytes);

/// <summary>
/// uavResourceDescの作成
/// </summary>
/// <param name="sizeInBytes"></param>
/// <returns></returns>
D3D12_RESOURCE_DESC CreateUavResourceDesc(size_t _sizeInBytes);

/// <summary>
/// DiscriptorHeapの作成
/// </summary>
/// <param name="device"></param>
/// <param name="heapType"></param>
/// <param name="numDescriptor"></param>
/// <param name="shaderVisible"></param>
/// <returns></returns>
/// template<typename T>
ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(Microsoft::WRL::ComPtr<ID3D12Device> _device,
	const D3D12_DESCRIPTOR_HEAP_TYPE& _heapType,
	const UINT& _numDescriptor,
	const bool& _shaderVisible);

/// <summary>
/// 
/// </summary>
/// <param name="device"></param>
/// <param name="sizeInBytes"></param>
/// <returns></returns>
ComPtr<ID3D12Resource> CreateBufferResource(ComPtr<ID3D12Device> _device, const size_t& _sizeInBytes);

/// <summary>
/// uavResourceの作成
/// </summary>
/// <param name="device"></param>
/// <param name="sizeInBytes"></param>
/// <returns></returns>
ComPtr<ID3D12Resource> CreateUAVResource(ComPtr<ID3D12Device> _device, const size_t& _sizeInBytes);

/// <summary>
/// srvResourceの作成
/// </summary>
/// <param name="device"></param>
/// <param name="sizeInBytes"></param>
/// <returns></returns>
ComPtr<ID3D12Resource> CreateSRVResource(ComPtr<ID3D12Device> _device, const size_t& _sizeInBytes);

/// <summary>
/// 深度情報を格納するリソースの生成
/// </summary>
/// <param name="device"></param>
/// <param name="width"></param>
/// <param name="height"></param>
/// <returns></returns>
ComPtr<ID3D12Resource> CreateDepthStencilTextureResource(ComPtr<ID3D12Device> _device, const int32_t& _width, const int32_t& _height);

/// <summary>
/// shaderResourceを作成する関数
/// </summary>
/// <param name="device">:デバイス</param>
/// <param name="resource">:Resourceのポインタ</param>
/// <param name="ResourceDesc">:リソースの設定</param>
/// <param name="heapProperties">:Heapの設定</param>
/// <param name="heapFlags">:Heapの特殊設定</param>
/// <param name="resourceState">:Resourceの状態</param>
/// <returns></returns>
ComPtr<ID3D12Resource> CerateShaderResource(ComPtr<ID3D12Device> _device,
						D3D12_RESOURCE_DESC* _resourceDesc,D3D12_HEAP_PROPERTIES* _heapProperties,
						const D3D12_HEAP_FLAGS& _heapFlags,const D3D12_RESOURCE_STATES& _resourceState
);

/// <summary>
/// CompileShader
/// </summary>
/// <param name=""></param>
ComPtr<IDxcBlob> CompilerShader(
	const std::wstring& _filePath,
	const wchar_t* _entryPoint,
	const wchar_t* _profile,
	ComPtr<IDxcUtils> _dxcUtils,
	ComPtr<IDxcCompiler3> _dxcCompiler,
	ComPtr<IDxcIncludeHandler> _includeHandler
);

/// <summary>
/// resourceのstateをstring型に変換する
/// </summary>
/// <param name="state"></param>
/// <returns></returns>
std::string ResourceStateToString(D3D12_RESOURCE_STATES _state);

/// <summary>
/// リソースのstateを変更する関数
/// </summary>
/// <param name="commandList">コマンドリスト</param>
/// <param name="resource">リソース</param>
/// <param name="beforeState">遷移前状態</param>
/// <param name="afterState">遷移後状態</param>
void TransitionResourceState(ID3D12GraphicsCommandList* _commandList, ID3D12Resource* _resource, D3D12_RESOURCE_STATES _beforeState, D3D12_RESOURCE_STATES _afterState);

/// <summary>
/// CPUHandleの作成
/// </summary>
/// <param name="descriptorHeap"></param>
/// <param name="descriptorSize"></param>
/// <param name="index"></param>
/// <returns></returns>
D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(ID3D12DescriptorHeap* _descriptorHeap, uint32_t _descriptorSize, uint32_t _index);

/// <summary>
/// GPUHandleの作成
/// </summary>
/// <param name="descriptorHeap"></param>
/// <param name="descriptorSize"></param>
/// <param name="index"></param>
/// <returns></returns>
D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(ID3D12DescriptorHeap* _descriptorHeap, uint32_t _descriptorSize, uint32_t _index);
