#pragma once

namespace AOENGINE {

/// <summary>
/// エディタウィンドウ用インターフェース
/// </summary>
class IEditorWindow {
public:

	IEditorWindow() = default;
	virtual ~IEditorWindow() = default;

public:

	virtual void InspectorWindow() = 0;

	virtual void HierarchyWindow() = 0;

	virtual void ExecutionWindow() = 0;

};

}