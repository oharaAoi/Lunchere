#pragma once
#include <memory>
#include <string>
#include "Engine/Module/Components/GameObject/BaseEntity.h"
#include "Engine/System/ShaderGraph/ShaderGraph.h"
#include "Engine/Lib/Json/IJsonConverter.h"

/// <summary>
/// ジェットエンジンのエフェクト
/// </summary>
class JetEngineBurn :
	public AOENGINE::BaseEntity {
public:

	/// <summary>
	/// ジェット噴射エフェクトのパラメータ
	/// </summary>
	struct Parameter : public AOENGINE::IJsonConverter {
		AOENGINE::Color color;
		Math::Vector3 scale;
		Math::Quaternion rotate;
		Math::Vector3 translate;
		float discardValue = 0.3f;
		std::string shaderGraphPath = "";
		
		Parameter() {
			SetGroupName("Effect");
			SetName("jetBurnParameter");
		}

		json ToJson(const std::string& id) const override {
			return AOENGINE::JsonBuilder(id)
				.Add("color", color)
				.Add("scale", scale)
				.Add("rotate", rotate)
				.Add("translate", translate)
				.Add("discardValue", discardValue)
				.Add("shaderGraphPath", shaderGraphPath)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			Convert::fromJson(jsonData, "color", color);
			Convert::fromJson(jsonData, "scale", scale);
			Convert::fromJson(jsonData, "rotate", rotate);
			Convert::fromJson(jsonData, "translate", translate);
			Convert::fromJson(jsonData, "discardValue", discardValue);
			Convert::fromJson(jsonData, "shaderGraphPath", shaderGraphPath);
		}

		void Debug_Gui() override;
	};

public:

	JetEngineBurn() = default;
	~JetEngineBurn() override;

public:

	void Finalize() {};

	// 初期化
	void Init(const std::string& _name);

	// 更新
	void Update();

	// 編集
	virtual void Debug_Gui() override;

private:

	std::unique_ptr<AOENGINE::ShaderGraph> shaderGraph_;

	Parameter param_;

	Math::Vector3 initScale_;
};
