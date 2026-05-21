#pragma once
#include <variant>
#include <string>
#include <stdexcept>
#include <nlohmann/json.hpp>

namespace AI {

template<typename T>
/// <summary>
/// Blackboard値の型情報
/// </summary>
struct ValueType {
	using type = T;
};

template<typename T>
/// <summary>
/// Blackboard値の型情報
/// </summary>
struct ValueType<T*> {
	using type = T;
};

template<typename T>
using ValueTypeT = typename ValueType<T>::type;

template<typename T>
decltype(auto) GetValue(T&& v) {
	if constexpr (std::is_pointer_v<std::decay_t<T>>) {
		return *v;
	} else {
		return v;
	}
}

/// <summary>
/// Blackboard内で使用する値
/// </summary>
struct BlackboardValue {
public: // data

	using Value = std::variant<
		int32_t, int32_t*,
		float, float*,
		bool, bool*,
		std::string, std::string*
	>;
	Value value_;

public: // constructor

	BlackboardValue() = default;
	BlackboardValue(Value v) : value_(v) {}
	~BlackboardValue() = default;

public: // public method

	void DebugValue(const std::string& name, BlackboardValue& wv);

	const Value& Get() const { return value_; }

	template<typename T>
	T As() const {
		T result{};

		std::visit([&](auto&& arg) {
			using U = std::decay_t<decltype(arg)>;

			if constexpr (std::is_same_v<U, T>) {
				result = arg;
			} else if constexpr (std::is_same_v<U, T*>) {
				result = *arg;
			}
				   }, value_);

		return result;
	}

	nlohmann::json to_json(const BlackboardValue& v) const;

	void from_json(const nlohmann::json& j, BlackboardValue& v);

};

}