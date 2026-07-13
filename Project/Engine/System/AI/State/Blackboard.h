#pragma once
#include <any>
#include <unordered_map>
#include <string>
#include <variant>
#include <stdexcept>
#include <vector>
// engine
#include "Engine/System/AI/State/BlackboardValue.h"

namespace AOENGINE {

namespace AI {

/// <summary>
/// Nodeごとに情報を共有するためのクラス
/// </summary>
class Blackboard {
public: // constructor

	Blackboard() = default;
	virtual ~Blackboard() = default;

public: // public method

	/// <summary>
	/// 参照の設定
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <param name="key"></param>
	/// <param name="ref"></param>
	template<typename T>
	void SetRef(const std::string& key, T& ref) {
		stateMap_[key] = BlackboardValue(&ref);
	}

	/// <summary>
	/// 編集処理
	/// </summary>
	void Debug_Gui();

	/// <summary>
	/// Keyのcombを表示
	/// </summary>
	/// <param name="_key"></param>
	/// <param name="index"></param>
	/// <param name="_label"></param>
	void KeyCombo(std::string& _key, int32_t& index, const std::string& _label);

	/// <summary>
	/// 読み込み処理
	/// </summary>
	/// <param name="_filePath"></param>
	void Load(const std::string& _filePath);

	/// <summary>
	/// 保存
	/// </summary>
	void Save();

private: // private method

	void CreateValue();

public: // accessor

	const std::string& GetPath() const { return path_; }

	/// <summary>
	/// 値の設定
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <param name="key"></param>
	/// <param name="value"></param>
	template<typename T>
	void Set(const std::string& key, const T& value) {
		stateMap_[key] = BlackboardValue(value);
	}

	/// <summary>
	/// 値の取得
	/// </summary>
	/// <param name="key"></param>
	/// <returns></returns>
	BlackboardValue Get(const std::string& key) const {
		auto it = stateMap_.find(key);
		if (it != stateMap_.end()) {
			return stateMap_.at(key).Get();
		}
		return BlackboardValue();
	}

protected: // protected variable

	std::unordered_map<std::string, BlackboardValue> stateMap_;

	std::string path_;

};

}

}

#ifndef AOENGINE_AI_NAMESPACE_ALIAS
#define AOENGINE_AI_NAMESPACE_ALIAS
namespace AI = AOENGINE::AI;
#endif
