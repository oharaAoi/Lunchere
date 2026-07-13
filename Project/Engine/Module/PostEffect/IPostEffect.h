#pragma once
#include <string>
#include "Engine/DirectX/Resource/DxResource.h"

namespace AOENGINE {
	class PostProcess;
}

namespace AOENGINE {

namespace PostEffect {

/// <summary>
/// PostEffectの基底クラス
/// </summary>
class IPostEffect {
public:

	IPostEffect() = default;
	virtual ~IPostEffect() = default;

	virtual void Init() = 0;

	virtual void PostInit(AOENGINE::PostProcess* ) {}

	virtual void SetCommand(ID3D12GraphicsCommandList* commandList, AOENGINE::DxResource* pingResource) = 0;

	virtual void CheckBox() = 0;

	virtual void ApplySaveSettings() = 0;

	virtual void Save(const std::string& rootField) = 0;

	virtual void Load(const std::string& rootField) = 0;

	virtual void Debug_Gui() = 0;

public:

	void SetIsEnable(bool _isEnable) { isEnable_ = _isEnable; }
	bool GetIsEnable() const { return isEnable_; }

protected:

	bool isEnable_ = false;

};

}

}

#ifndef AOENGINE_POSTEFFECT_NAMESPACE_ALIAS
#define AOENGINE_POSTEFFECT_NAMESPACE_ALIAS
namespace PostEffect = AOENGINE::PostEffect;
#endif
