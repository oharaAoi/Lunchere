#pragma once
#include <list>
#include <memory>
#include <vector>
#include <string>
#include "Engine/Lib/Math/MathStructures.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;
namespace AOENGINE {

/// <summary>
/// Sceneを読み込みクラス
/// </summary>
class SceneLoader {
public:		// data

	/// <summary>
	/// ファイルに記載されている各objectの情報
	/// </summary>
	struct ColliderData {
		std::string colliderType = "";
		std::string colliderTag = "";
		Math::Vector3 center;
		Math::Vector3 size;
		float radius;
		std::vector<std::string> filter;
	};

	/// <summary>
	/// マテリアル読み込み情報
	/// </summary>
	struct MaterialData {
		std::string shader = "";
		float roughness;
		float metallic;
		float iblStrength;
		float shininess;

		std::string normalMap = "";
	};

	/// <summary>
	/// シーンオブジェクト読み込み情報
	/// </summary>
	struct Objects {
		std::string name;
		std::string modelName;
		Math::QuaternionSRT srt;

		std::vector<ColliderData> collidersData;

		bool isRendering_;

		std::list<Objects> children;

		MaterialData material;
	};

	/// <summary>
	/// 各objectをまとめたデータ
	/// </summary>
	struct LevelData {
		std::vector<Objects> objects;
	};

public:

	SceneLoader() = default;
	~SceneLoader() = default;
	SceneLoader(const SceneLoader&) = delete;
	const SceneLoader& operator=(const SceneLoader&) = delete;

	static SceneLoader* GetInstance();

	/// <summary>
	/// 初期化処理
	/// </summary>
	void Init();

	/// <summary>
	/// Scene読み込み処理
	/// </summary>
	/// <param name="directory"></param>
	/// <param name="fileName"></param>
	/// <param name="extension"></param>
	void Load(const std::string& directory, const std::string& fileName, const std::string& extension);

	/// <summary>
	/// objectの読み込み処理
	/// </summary>
	/// <param name="objectJson"></param>
	/// <returns></returns>
	Objects LoadObject(const json& objectJson);

	/// <summary>
	/// ファイル変更を監視する
	/// </summary>
	bool MonitorFileChange();

public:

	Objects GetObjects(const std::string& objName) const;

	LevelData* GetLevelData() const { return levelData_.get(); }

private:

	std::unique_ptr<LevelData> levelData_;

	std::string directory_;
	std::string fileName_;
	std::string extension_;

};

}