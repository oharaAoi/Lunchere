#pragma once
#include <string>
#include "Engine/DirectX/Utilities/DirectXUtils.h"
#include "Engine/DirectX/Resource/DxResource.h"
#include "Engine/System/ShaderGraph/ShaderGraph.h"
#include "Engine/DirectX/Pipeline/Pipeline.h"
#include "Engine/Module/Components/Materials/MaterialStructures.h"
#include "Engine/Lib/Color.h"
#include "Engine/Lib/Math/Vector3.h"
#include "Engine/Lib/Math/MathStructures.h"
#include "Engine/Module/Components/IComponent.h"

enum class MaterialShaderType {
	UniversalRender,	// 一般的なレンダリング
	ShaderGraphRender,	// shaderGraphを利用したレンダリング
};

namespace AOENGINE {

/// <summary>
/// BaseとなるMaterial
/// </summary>
class BaseMaterial :
	public IComponent {
public:

	BaseMaterial() = default;
	virtual ~BaseMaterial() override = default;

public:

	// 初期化
	virtual void Init() = 0;
	// 更新
	virtual void Update() = 0;
	// 編集処理
	virtual void Debug_Gui() = 0;
	// materialの設定
	virtual void SetMaterialData(ModelMaterialData materialData) = 0;
	// コマンドに積む
	virtual void BindCommand(ID3D12GraphicsCommandList* _cmdList, const AOENGINE::Pipeline* _pso) = 0;


protected:

	/// <summary>
	/// UVを編集する
	/// </summary>
	void EditUV();

	/// <summary>
	/// ShaderTypeを編集する
	/// </summary>
	void EditShaderType();

public:

	D3D12_GPU_VIRTUAL_ADDRESS GetBufferAddress() const { return cBuffer_->GetGPUVirtualAddress(); }

	void SetShaderType(MaterialShaderType _type) { shaderType_ = _type; }
	MaterialShaderType GetShaderType() const { return shaderType_; }

	void SetUvScale(const Math::Vector3& _scale) { uvTransform_.scale = _scale; }
	void SetUvRotate(const Math::Vector3& _rotate) { uvTransform_.rotate = _rotate; }
	void SetUvTranslate(const Math::Vector3& _translate) { uvTransform_.translate = _translate; }

	void SetAlbedoTexture(const std::string& _name) { textureName_ = _name; }
	const std::string GetAlbedoTexture() const { return textureName_; }

	void SetShaderGraph(ShaderGraph* _graph);
	AOENGINE::ShaderGraph* GetShaderGraph() const { return shaderGraph_; }

	void SetColor(const AOENGINE::Color& _color) { color_ = _color; }
	const AOENGINE::Color GetColor() const { return color_; }

	void SetAlpha(float _alpha) { color_.a = _alpha; };
	void SetIsLighting(bool _isLighting) { isLighting_ = _isLighting; }

	void SetDiscardValue(float _value) { discardValue_ = _value; }

protected:

	MaterialShaderType shaderType_ = MaterialShaderType::UniversalRender;
	int shaderTypeIndex_ = 0;

	ComPtr<ID3D12Resource> cBuffer_;

	Math::SRT uvTransform_ = Math::SRT();
	std::string textureName_ = "white.png";

	AOENGINE::ShaderGraph* shaderGraph_ = nullptr;

	AOENGINE::Color color_;
	bool isLighting_ = true;

	float discardValue_ = 0.01f;

};

}