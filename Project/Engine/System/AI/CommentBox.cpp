#include "CommentBox.h"
#include "Engine/Utilities/ImGuiHelperFunc.h"
#include "Engine/System/Input/Input.h"
#include <cstring>

uint32_t CommentBox::nextId_ = 10000;
constexpr size_t kCommentCapacity = 512;

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　初期化処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void CommentBox::Init(const ImVec2& _min, const ImVec2& _max, const std::string& _text) {
	id_ = nextId_;
	nextId_++;

	min_ = _min;
	max_ = _max;
	size_ = max_ - min_;

	text_ = _text;
	
	ax::NodeEditor::SetNodePosition(id_, min_);
	ax::NodeEditor::SetGroupSize(id_, size_);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　更新処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void CommentBox::Update() {
	if (isSelect_) {
		if (AOENGINE::Input::GetInstance()->IsTriggerKey(DIK_DELETE)) {
			isDelete_ = true;
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　描画処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void CommentBox::Draw() {
	constexpr float alpha = 0.75f;

	isSelect_ = ax::NodeEditor::IsNodeSelected(id_);

	// -----------------------------
	// ノード本体（当たり判定）
	// -----------------------------
	ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
	ax::NodeEditor::PushStyleColor(ax::NodeEditor::StyleColor_NodeBg, ImColor(60, 60, 60, 100));
	ax::NodeEditor::PushStyleColor(ax::NodeEditor::StyleColor_NodeBorder, ImColor(120, 120, 120, 120));

	ax::NodeEditor::BeginNode(id_);

	std::string guiId = "##comment" + std::to_string(id_);
	InputTextWithString("input", guiId.c_str(), text_);
	ImVec2 groupCursorPos = ImGui::GetCursorScreenPos();
	ax::NodeEditor::Group(size_);
	bool groupSubmitted = ImGui::GetCursorScreenPos().x != groupCursorPos.x ||
		ImGui::GetCursorScreenPos().y != groupCursorPos.y;
	ImVec2 groupSize = ImGui::GetItemRectSize();

	ax::NodeEditor::EndNode();

	if (ax::NodeEditor::BeginGroupHint(id_)) {
		auto bgAlpha = static_cast<int>(ImGui::GetStyle().Alpha * 255);

		auto min = ax::NodeEditor::GetGroupMin();
		
		ImGui::SetCursorScreenPos(min - ImVec2(-8, ImGui::GetTextLineHeightWithSpacing() + 4));
		ImGui::BeginGroup();
		ImGui::TextUnformatted(text_.c_str());
		ImGui::EndGroup();

		auto drawList = ax::NodeEditor::GetHintBackgroundDrawList();

		ImRect hintBounds(
			ImGui::GetItemRectMin(),
			ImGui::GetItemRectMax()
		);

		ImRect hintFrameBounds(
			hintBounds.Min - ImVec2(8, 4),
			hintBounds.Max + ImVec2(8, 4)
		);

		drawList->AddRectFilled(
			hintFrameBounds.Min,
			hintFrameBounds.Max,
			IM_COL32(255, 255, 255, 64 * bgAlpha / 255),
			4.0f
		);

		drawList->AddRect(
			hintFrameBounds.Min,
			hintFrameBounds.Max,
			IM_COL32(255, 255, 255, 128 * bgAlpha / 255),
			4.0f
		);
	}
	ax::NodeEditor::EndGroupHint();
	
	ax::NodeEditor::PopStyleColor(2);
	ImGui::PopStyleVar();

	if (groupSubmitted && groupSize.x > 0.0f && groupSize.y > 0.0f) {
		size_ = groupSize;
		min_ = ax::NodeEditor::GetNodePosition(id_);
		max_ = min_ + size_;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　jsonに変換する
//////////////////////////////////////////////////////////////////////////////////////////////////

nlohmann::json CommentBox::ToJson() {
	nlohmann::json result;
	result["id"] = id_;
	// min はノードの位置
	result["min"] = nlohmann::json{ {"x", min_.x}, {"y", min_.y} };
	// size を直接保存 (中身のサイズ)
	result["size"] = nlohmann::json{ {"x", size_.x}, {"y", size_.y} };
	result["text"] = text_;
	return result;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　jsonの情報からnodeに適応する
//////////////////////////////////////////////////////////////////////////////////////////////////

void CommentBox::FromJson(const nlohmann::json& _json) {
	id_ = _json["id"];
	min_ = ImVec2(_json["min"]["x"], _json["min"]["y"]);

	// size を直接復元
	if (_json.contains("size")) {
		size_ = ImVec2(_json["size"]["x"], _json["size"]["y"]);
	} else {
		// 互換性維持: 古いデータ(maxがある場合)への対応
		if (_json.contains("max")) {
			ImVec2 oldMax = ImVec2(_json["max"]["x"], _json["max"]["y"]);
			size_ = oldMax - min_;
		}
	}

	// max は計算で求める
	max_ = min_ + size_;

	text_ = _json["text"];

	// ノードエディタ上の位置を設定
	ax::NodeEditor::SetNodePosition(id_, min_);
	ax::NodeEditor::SetGroupSize(id_, size_);

	// 【重要】ID競合の回避
	// ロードしたIDが現在のカウンター以上なら、カウンターを進める
	if (id_ >= nextId_) {
		nextId_ = id_ + 1;
	}
}

ImRect CommentBox::ExpandRect(const ImRect& r, float x, float y) {
	return ImRect(
		ImVec2(r.Min.x - x, r.Min.y - y),
		ImVec2(r.Max.x + x, r.Max.y + y)
	);
}
