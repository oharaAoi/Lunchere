#pragma once
#include "Engine/System/ShaderGraph/Node/BaseShaderGraphNode.h"
#include "Engine/Lib/Color.h"

namespace AOENGINE {

template<typename T>
/// <summary>
/// 入出力優先度ノードクラス
/// </summary>
class InOutPriorityNode :
	public BaseShaderGraphNode {
public: // コンストラクタ

	InOutPriorityNode() = default;
	~InOutPriorityNode() override = default;

public:

	/// <summary>
	/// 初期化関数
	/// </summary>
	void Init() override;

	/// <summary>
	/// 更新関数
	/// </summary>
	void customUpdate() override;

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

	T inputValue_;
	T value_;

};

template<typename T>
inline void InOutPriorityNode<T>::Init() {
	// 入力の設定
	if constexpr (std::is_same_v<T, float>) {
		addIN<float>("x", inputValue_, ImFlow::ConnectionFilter::SameType());

	} else if constexpr (std::is_same_v<T, Math::Vector2>) {
		addIN<float>("x", inputValue_.x, ImFlow::ConnectionFilter::SameType());
		addIN<float>("y", inputValue_.y, ImFlow::ConnectionFilter::SameType());

	} else if constexpr (std::is_same_v<T, Math::Vector3>) {
		addIN<float>("x", inputValue_.x, ImFlow::ConnectionFilter::SameType());
		addIN<float>("y", inputValue_.y, ImFlow::ConnectionFilter::SameType());
		addIN<float>("z", inputValue_.z, ImFlow::ConnectionFilter::SameType());

	} else if constexpr (std::is_same_v<T, Math::Vector4>) {
		addIN<float>("x", inputValue_.x, ImFlow::ConnectionFilter::SameType());
		addIN<float>("y", inputValue_.y, ImFlow::ConnectionFilter::SameType());
		addIN<float>("z", inputValue_.z, ImFlow::ConnectionFilter::SameType());
		addIN<float>("w", inputValue_.w, ImFlow::ConnectionFilter::SameType());

	} else if constexpr (std::is_same_v<T, AOENGINE::Color>) {
		addIN<AOENGINE::Color>("color", inputValue_, ImFlow::ConnectionFilter::SameType());
	}

	// outputの設定
	auto texOut = addOUT<T>("output", ImFlow::PinStyle::blue());
	texOut->behaviour([this]() { return value_; });
}

template<typename T>
inline void InOutPriorityNode<T>::customUpdate() {
	// 入力の受取
	if constexpr (std::is_same_v<T, float>) {
		inputValue_ = getInVal<float>("x");

	} else if constexpr (std::is_same_v<T, Math::Vector2>) {
		inputValue_.x = getInVal<float>("x");
		inputValue_.y = getInVal<float>("y");

	} else if constexpr (std::is_same_v<T, Math::Vector3>) {
		inputValue_.x = getInVal<float>("x");
		inputValue_.y = getInVal<float>("y");
		inputValue_.z = getInVal<float>("z");

	} else if constexpr (std::is_same_v<T, Math::Vector4>) {
		inputValue_.x = getInVal<float>("x");
		inputValue_.y = getInVal<float>("y");
		inputValue_.z = getInVal<float>("z");
		inputValue_.w = getInVal<float>("w");

	} else if constexpr (std::is_same_v<T, AOENGINE::Color>) {
		inputValue_ = getInVal<AOENGINE::Color>("color");
	}

	// 値の更新
	value_ = inputValue_;
}

template<typename T>
inline void InOutPriorityNode<T>::draw() {
}

template<typename T>
inline nlohmann::json InOutPriorityNode<T>::toJson() {
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
inline void InOutPriorityNode<T>::fromJson(const nlohmann::json& _json) {
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