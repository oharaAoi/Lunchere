#include "JsonItems.h"
#include <iostream>
#include <fstream>

namespace fs = std::filesystem;
using namespace AOENGINE;

const std::string JsonItems::sDirectoryPath_ = "./Project/Packages/Game/GameData/JsonItems/";

JsonItems* JsonItems::GetInstance() {
	static JsonItems instance;
	return &instance;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　初期化処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void JsonItems::Init() {
	// ------------------------------------------
	// ↓ ディレクトリがなければ作成する
	// ------------------------------------------
	std::filesystem::path dire(sDirectoryPath_);
	if (!std::filesystem::exists(sDirectoryPath_)) {
		std::filesystem::create_directories(sDirectoryPath_);
	}

	LoadAllFile();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　読み込みを行う
//////////////////////////////////////////////////////////////////////////////////////////////////

void JsonItems::LoadAllFile() {
	jsonMap_.clear();
	// objectの保存項目の読み込み
	std::filesystem::directory_iterator rootDir(sDirectoryPath_ + "Object");
	for (const fs::directory_entry& entryDir : fs::directory_iterator(rootDir)) {
		if (entryDir.is_directory()) {
			// サブディレクトリの名前を取得
			const fs::path& subDirPath = entryDir.path();

			for (const fs::directory_entry& subEntry : fs::directory_iterator(subDirPath)) {
				// サブディレクトリ内のファイルパスを取得
				const fs::path& filePath = subEntry.path();
				// ファイル拡張子を取得
				std::string extension = filePath.extension().string();

				// .jsonファイル以外はスキップ
				if (extension.compare(".json") != 0) {
					continue;
				}

				Load(subDirPath.stem().string(), filePath.stem().string(), sDirectoryPath_ + "Object/");
			}
		}
	}

	// effect読み込み
	std::filesystem::directory_iterator rootEffectDir(sDirectoryPath_  + "Effect");
	for (const fs::directory_entry& entryDir : fs::directory_iterator(rootEffectDir)) {
		if (entryDir.is_directory()) {
			// サブディレクトリの名前を取得
			const fs::path& subDirPath = entryDir.path();

			for (const fs::directory_entry& subEntry : fs::directory_iterator(subDirPath)) {
				// サブディレクトリ内のファイルパスを取得
				const fs::path& filePath = subEntry.path();
				// ファイル拡張子を取得
				std::string extension = filePath.extension().string();

				// .jsonファイル以外はスキップ
				if (extension.compare(".json") != 0) {
					continue;
				}

				Load(subDirPath.stem().string(), filePath.stem().string(), sDirectoryPath_ + "Effect/");
			}
		}
	}
}

void AOENGINE::JsonItems::LoadDesignationPath(const std::string& directory, const std::string& _rootFold) {
	std::filesystem::directory_iterator rootEffectDir(directory + _rootFold);
	for (const fs::directory_entry& entryDir : fs::directory_iterator(rootEffectDir)) {
		if (entryDir.is_directory()) {
			// サブディレクトリの名前を取得
			const fs::path& subDirPath = entryDir.path();

			for (const fs::directory_entry& subEntry : fs::directory_iterator(subDirPath)) {
				// サブディレクトリ内のファイルパスを取得
				const fs::path& filePath = subEntry.path();
				// ファイル拡張子を取得
				std::string extension = filePath.extension().string();

				// .jsonファイル以外はスキップ
				if (extension.compare(".json") != 0) {
					continue;
				}

				Load(subDirPath.stem().string(), filePath.stem().string(), directory);
			}
		}
	}
}

void JsonItems::SaveAllFile() {
	for (const auto& [groupId, converterGroup] : jsonConverterMap_) {
		json groupResult;
		for (const auto& [funcName, func] : converterGroup.items) {
			Save(groupId, func(converterGroup.key));
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　Groupの追加
//////////////////////////////////////////////////////////////////////////////////////////////////

void JsonItems::AddGroup(const std::string& _groupName, const json& _jsonData) {
	std::string rootKey;
	if (_jsonData.is_object() && !_jsonData.empty()) {
		rootKey = _jsonData.begin().key();
	}
	
	jsonMap_[_groupName].items[rootKey] = _jsonData;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　保存を行う
//////////////////////////////////////////////////////////////////////////////////////////////////

void JsonItems::Save(const std::string& _groupName, const json& _saveData, const std::string& directory, const std::string& _rootFold) {
	// 最上位キーの取得
	if (_saveData.is_object() && !_saveData.empty()) {
		// 最上位のキーの名前をファイル名とする
		std::string rootKey = _saveData.begin().key();
		std::string filePath;
		std::filesystem::path dirPath;
		// ファイルパスの作成
		if (_rootFold == "") {
			filePath = directory + _groupName + "/" + rootKey + ".json";
			dirPath = std::filesystem::path(directory + _groupName);
		} else {
			filePath = directory + _rootFold + "/" + _groupName + "/" + rootKey + ".json";
			dirPath = std::filesystem::path(directory + _rootFold + "/" + _groupName);
		}
		

		// -------------------------------------------------
		// ↓ ディレクトリがなければ作成を行う
		// -------------------------------------------------
		if (!std::filesystem::exists(dirPath)) {
			std::filesystem::create_directories(dirPath);
			std::cout << "Created directory: " << dirPath << std::endl;
		}

		// -------------------------------------------------
		// ↓ ファイルを開けるかのチェックを行う
		// -------------------------------------------------
		std::ofstream outFile(filePath);
		if (outFile.fail()) {
			std::string message = "Faild open data file for write\n";
			//Log(message);
			assert(0);
			return;
		}

		// -------------------------------------------------
		// ↓ ファイルに実際に書き込む
		// -------------------------------------------------
		outFile << std::setw(4) << _saveData << std::endl;
		outFile.close();

	} else {
		//Log("Invalid or empty JSON data\n");
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　読み込みを行う
//////////////////////////////////////////////////////////////////////////////////////////////////

void JsonItems::Load(const std::string& _groupName, const std::string& _rootKey, const std::string& directory, const std::string& _rootFold) {
	std::string filePath;
	// ファイルパスの作成
	if (_rootFold == "") {
		filePath = directory + _groupName + "/" + _rootKey + ".json";
	} else {
		filePath = directory + _rootFold + "/" + _groupName + "/" + _rootKey + ".json";
	}

	// 読み込み用ファイルストリーム
	std::ifstream ifs;
	// ファイルを読み込みように開く
	ifs.open(filePath);

	if (ifs.fail()) {
		std::string message = "not Exist " + filePath + ".json";
		assert(0);
		return;
	}

	json root;
	// json文字列からjsonのデータ構造に展開
	ifs >> root;
	// ファイルを閉じる
	ifs.close();

	// 値の追加
	GetInstance()->AddGroup(_groupName, root);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　値を取得する
//////////////////////////////////////////////////////////////////////////////////////////////////

json JsonItems::GetData(const std::string& _groupName, const std::string& _rootKey) {
	return GetInstance()->GetValue(_groupName, _rootKey);
}

json JsonItems::GetValue(const std::string& _groupName, const std::string& _rootKey) {
	if (jsonMap_.contains(_groupName) && jsonMap_[_groupName].items.contains(_rootKey)) {
		return jsonMap_[_groupName].items[_rootKey];
	}
	return nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　各構造体で宣言した保存関数をmapに格納しておく
//////////////////////////////////////////////////////////////////////////////////////////////////

void JsonItems::AddConverter(const std::string& _groupName, const std::string& _rootKey, std::function<json(const std::string&)> _function) {
	GetInstance()->AddConverterGroup(_groupName, _rootKey, _function);
}

void JsonItems::AddConverterGroup(const std::string& _groupName, const std::string& _rootKey, std::function<json(const std::string&)> _function) {
	jsonConverterMap_[_groupName].items[_rootKey] = _function;
	jsonConverterMap_[_groupName].key = _rootKey;
}

