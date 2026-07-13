#pragma once
#include "Engine/Lib/Math/MyMath.h"

#include <string>
#include <vector>
#include <json.hpp>
#include <Engine/System/Manager/ImGuiManager.h>

namespace AOENGINE {

/// <summary>
/// コメントボックスの表示
/// </summary>
class CommentBox {
public: // constructor

	CommentBox() = default;
	~CommentBox() = default;

public: // public method

	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="_min"></param>
	/// <param name="_max"></param>
	/// <param name="_text"></param>
	void Init(const ImVec2& _min, const ImVec2& _max, const std::string& _text = "Comment Box Text");

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

	/// <summary>
	/// jsonに変換する
	/// </summary>
	/// <returns></returns>
	nlohmann::json ToJson();

	/// <summary>
	/// jsonの情報からnodeに適応する
	/// </summary>
	/// <param name="_json"></param>
	void FromJson(const nlohmann::json& _json);

private: // private method

	ImRect ExpandRect(const ImRect& r, float x, float y);

public: // accessor method

	bool GetIsSelect() const { return isSelect_; }
	bool GetIsDelete() const { return isDelete_; }

private: // private variable

	uint32_t id_;
	static uint32_t nextId_;

	ImVec2 min_;
	ImVec2 max_;
	ImVec2 size_;

	std::string text_ = "Comment Box Text";
	ImU32 bgColor_ = IM_COL32(255, 255, 128, 128);

	bool dragging_ = false;
	bool resizing_ = false;

	bool isSelect_ = false;
	bool isDelete_ = false;

	ImVec2 prevPos_;
};

}

using AOENGINE::CommentBox;

