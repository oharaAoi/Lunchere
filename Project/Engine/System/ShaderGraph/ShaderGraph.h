#pragma once
#include <memory>
#include <string>
#include <unordered_set>
#include "Engine/Module/Components/Attribute/AttributeGui.h"
#include "Engine/System/Manager/ImGuiManager.h"
#include "Engine/System/ShaderGraph/ShaderGraphNodeFactory.h"
#include "Engine/Lib/Json/IJsonConverter.h"

namespace AOENGINE {

/// <summary>
/// ShaderGraphクラス
/// </summary>
class ShaderGraph :
	public AOENGINE::AttributeGui {
public:

	/// <summary>
	/// ShaderGraphのファイルパス保存用構造体
	/// </summary>
	struct FilePathParam : public AOENGINE::IJsonConverter {
		std::string path = "";

		FilePathParam() {
			SetGroupName("ShaderGraph");
			SetName("graph");
		}

		json ToJson(const std::string& id) const override {
			return AOENGINE::JsonBuilder(id)
				.Add("path", path)
				.Build();
		}

		void FromJson(const json& jsonData) override {
			Convert::fromJson(jsonData, "path", path);
		}

		void Debug_Gui() override {};
	};

public: // コンストラクタ

	ShaderGraph() = default;
	~ShaderGraph() override;

public:

	/// <summary>
	/// 初期化関数
	/// </summary>
	void Init(const std::string& _name);

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// Nodeの更新を実行する
	/// </summary>
	/// <param name="node"></param>
	/// <param name="visited"></param>
	void ExecuteFrom(ImFlow::BaseNode* node, std::unordered_set<ImFlow::BaseNode*>& visited);

	/// <summary>
	/// 編集処理
	/// </summary>
	void Debug_Gui();

	/// <summary>
	/// 読み込み
	/// </summary>
	/// <param name="_filePath"></param>
	void Load(const std::string& _filePath);

public:

	AOENGINE::DxResource* GetResource() const;

	const std::string& GetPath() const { return param_.path; }

private:

	// editor関連
	ShaderGraphNodeFactory nodeFactory_;
	std::unique_ptr<ImFlow::ImNodeFlow> editor_;

	// 最終的なNodeのポインタ
	std::shared_ptr<ShaderGraphResultNode> resultNode_ = nullptr;

	// pathを持ったパラメータ
	FilePathParam param_;

};

}