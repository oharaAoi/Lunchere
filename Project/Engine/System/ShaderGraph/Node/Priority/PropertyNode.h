#pragma once
#include "Engine/System/ShaderGraph/Node/BaseShaderGraphNode.h"
#include "Engine/Utilities/ImGuiHelperFunc.h"

namespace AOENGINE {

/// <summary>
/// floatやMath::Vector2などのtemplateに基本の型のNode
/// </summary>
/// <typeparam name="T"></typeparam>
template<typename T>
/// <summary>
/// プロパティノードクラス
/// </summary>
class PropertyNode :
	public BaseShaderGraphNode {
public: // コンストラクタ

	PropertyNode() = default;
	PropertyNode(const std::string& _variableType) : variableType_(_variableType) {};
	~PropertyNode() override = default;

public:

	/// <summary>
	/// 初期化関数
	/// </summary>
	void Init() override;

	/// <summary>
	/// 更新関数
	/// </summary>
	void customUpdate() override {};

	/// <summary>
	/// guiの更新
	/// </summary>
	void updateGui() override {};

	/// <summary>
	/// Node描画
	/// </summary>
	void draw() override;

	/// <summary>
	/// json形式にする
	/// </summary>
	/// <returns></returns>
	nlohmann::json toJson() override;

	/// <summary>
	/// json形式から情報を設定する
	/// </summary>
	/// <param name="_json"></param>
	void fromJson(const nlohmann::json& _json) override;

private:

	T value_;
	std::string variableType_;
};

template<typename T>
inline void PropertyNode<T>::Init() {
	// outputの設定
	auto texOut = addOUT<T>("output", ImFlow::PinStyle::blue());
	texOut->behaviour([this]() { return value_; });
}

template<typename T>
inline void PropertyNode<T>::draw() {
	// -------- 内部プレビュー ----------
	ImGui::Spacing();
	ImGui::SetNextItemWidth(150);
	TemplateValueGui("value", value_);
}

template<typename T>
inline nlohmann::json PropertyNode<T>::toJson() {
	nlohmann::json result;
	BaseInfoToJson(result);
	if constexpr (std::is_same_v<T, float>) {
		result["props"]["value"] = { value_ };
	} else if constexpr (std::is_same_v<T, Math::Vector2>) {
		result["props"]["value"] = { value_.x, value_.y };
	} else if constexpr (std::is_same_v<T, Math::Vector3>) {
		result["props"]["value"] = { value_.x, value_.y, value_.z };
	} else if constexpr (std::is_same_v<T, Math::Vector4>) {
		result["props"]["value"] = { value_.x, value_.y, value_.z, value_.w };
	} else if constexpr (std::is_same_v<T, AOENGINE::Color>) {
		result["props"]["color"] = { value_.r, value_.g, value_.b, value_.a };
	}
	return result;
}

template<typename T>
inline void PropertyNode<T>::fromJson(const nlohmann::json& _json) {
	BaseInfoFromJson(_json);
	if constexpr (std::is_same_v<T, float>) {
		value_ = _json.at("props").at("value").at(0).get<float>();
	} else if constexpr (std::is_same_v<T, Math::Vector2>) {
		auto& value = _json.at("props").at("value");
		value_.x = value.at(0).get<float>();
		value_.y = value.at(1).get<float>();
	} else if constexpr (std::is_same_v<T, Math::Vector3>) {
		auto& value = _json.at("props").at("value");
		value_.x = value.at(0).get<float>();
		value_.y = value.at(1).get<float>();
		value_.z = value.at(2).get<float>();
	} else if constexpr (std::is_same_v<T, Math::Vector4>) {
		auto& value = _json.at("props").at("value");
		value_.x = value.at(0).get<float>();
		value_.y = value.at(1).get<float>();
		value_.z = value.at(2).get<float>();
		value_.w = value.at(3).get<float>();
	} else if constexpr (std::is_same_v<T, AOENGINE::Color>) {
		auto& value = _json.at("props").at("color");
		value_.r = value.at(0).get<float>();
		value_.g = value.at(1).get<float>();
		value_.b = value.at(2).get<float>();
		value_.a = value.at(3).get<float>();
	}
}

}