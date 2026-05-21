#pragma once
#include "Engine/DirectX/Descriptor/DescriptorHeap.h"
#include "Engine/DirectX/Utilities/DirectXUtils.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "Externals/ImGui/imgui.h"
#include "Externals/ImGui/imgui_impl_dx12.h"
#include "Externals/ImGui/imgui_impl_win32.h"
#include "Flow/ImNodeFlow.h"
#include "ImGuizmo.h"
#include "ImGuiFileDialog.h"
#include "imgui_node_editor.h"
#include "Engine/Lib/Math/Vector2.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace AOENGINE {

/// <summary>
/// ImGui管理クラス
/// </summary>
class ImGuiManager {

public: // 静的メンバ関数

	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	/// <returns></returns>
	static ImGuiManager* GetInstance();

	static bool ImGuiHandler(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

public:

	ImGuiManager() = default;
	~ImGuiManager() = default;
	ImGuiManager(const ImGuiManager&) = delete;
	const ImGuiManager& operator=(const ImGuiManager&) = delete;

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="winApp"></param>
	/// <param name="dxCommon"></param>
	void Init(HWND hwnd, ID3D12Device* device, uint32_t bufferCount, ID3D12DescriptorHeap* srvHeap);

	/// <summary>
	/// 終了処理
	/// </summary>
	void Finalize();

	/// <summary>
	/// 使う
	/// </summary>
	void Begin();

	/// <summary>
	/// ゲームの処理が終わった後
	/// </summary>
	void End();

	void UpdateMousePosition(HWND _winHwnd, const Math::Vector2& _renderTargetSize);

	/// <summary>
	/// 描画する
	/// </summary>
	void Draw(ID3D12GraphicsCommandList* commandList);
};
}