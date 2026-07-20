#pragma once
#include <memory>
// Engine
#include "Engine/Module/Components/GameObject/BaseEntity.h"
#include "Engine/System/ShaderGraph/ShaderGraph.h"
#include "Engine/Lib/Json/IJsonConverter.h"

/// <summary>
/// Laserを覆う円柱エフェト
/// </summary>
class LaserCylinder :
	public AOENGINE::BaseEntity {
public:

	/// <summary>
	/// レーザー円柱エフェクトの表示パラメータ
	/// </summary>
	struct LaserParameter : public AOENGINE::IJsonConverter {
		Math::Vector3 scale;
		
		LaserParameter() {
			SetGroupName("Bullet");
			SetName("LaserCylinderParameter");
		}

		json ToJson(const std::string& id) const override {
			return AOENGINE::JsonBuilder(id)
				.Add("scale", scale)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			Convert::fromJson(jsonData, "scale", scale);
		}

		void Debug_Gui() override {};
	};
public: // コンストラクタ

	LaserCylinder() = default;
	~LaserCylinder();

public:

	// 終了処理
	void Finalize() {};

	// 初期化
	void Init(const AOENGINE::Color& _color);

	// 更新
	void Update();

	// 編集
	virtual void Debug_Gui() override;

	void SetUvScale(float _scaleZ);
	void SetScaleZ(float _scaleZ);

public:

	AOENGINE::ShaderGraph* GetShaderGraph() { return shaderGraph_.get(); }

	const std::string& GetShaderGraphPath() const { return shaderGraph_->GetPath(); }

private:

	std::unique_ptr<AOENGINE::ShaderGraph> shaderGraph_;

	LaserParameter param_;

};

