#pragma once
#include "dxgidebug.h"
#include <wrl/client.h>
#pragma comment(lib, "dxguid.lib")
#include <string>
#include <dxgi1_3.h>

/// <summary>
/// D3Dリソースリーク確認用構造体
/// </summary>
struct D3DResourceLeakChecker {
	~D3DResourceLeakChecker() {
		Microsoft::WRL::ComPtr<IDXGIDebug1> debug;
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug)))) {
			debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
			debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
			debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
		}
	}
};

/// <summary>
/// Logを出すクラス
/// </summary>
class Debug {
public:

	Debug() = default;
	~Debug() = default;

	void Log(const std::string& message);

};