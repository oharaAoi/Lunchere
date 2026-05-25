#include "BehaviorTree.h"
#include <filesystem>
#include "Engine/Utilities/Logger.h"
#include "Engine/System/AI/Node/BehaviorRootNode.h"
#include "Engine/System/AI/BehaviorTreeSerializer.h"
#include "Engine/System/AI/BehaviorTreeNodeFactory.h"

using namespace AOENGINE;
using namespace AI;

BehaviorTree::~BehaviorTree() {
	nodeList_.clear();
	creators_.clear();
	editor_.Finalize();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　初期化処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void BehaviorTree::Init() {
	isExecute_ = true;
	isOpenEditor_ = false;

	root_ = nodeList_.emplace_back(std::make_unique<BehaviorRootNode>()).get();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　Tree処理
//////////////////////////////////////////////////////////////////////////////////////////////////

bool BehaviorTree::Run() {
	if (root_ == nullptr) { return false; }

	// すべてのnodeの更新を走らせる
	for (auto it = nodeList_.begin(); it != nodeList_.end();) {
		(*it)->Update();
		it++;
	}

	if (!isExecute_) { return false; }
	// nodeの内容を実行させる
	if (root_ != nullptr) {
		rootState_ = root_->Execute();
		if (rootState_ == BehaviorStatus::Failure) {
			AOENGINE::Logger::Log("RootNodeが失敗を返しました");
			return false;
		}
	}
	return true;
}

void BehaviorTree::SetCanTaskMap(const std::unordered_map<std::string, ActionNode>& _creators) {
	creators_ = _creators;
}

void BehaviorTree::AddGoal(std::shared_ptr<IOrientedGoal> _goal) {
	auto goal = goalArray_.emplace_back(std::move(_goal));
	goal->SetBlackboard(blackboard_);
}

void BehaviorTree::DisplayState(const ImVec2& _pos, float _aggressionScore, const ImVec2& _offset) {
	if (ImGui::Begin("Game Window", nullptr)) {
		ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar
			| ImGuiWindowFlags_AlwaysAutoResize
			| ImGuiWindowFlags_NoScrollbar
			| ImGuiWindowFlags_NoCollapse
			| ImGuiWindowFlags_NoDocking
			| ImGuiWindowFlags_NoBackground;
		ImGui::SetNextWindowPos(_pos + _offset, ImGuiCond_Always);
		if (ImGui::Begin("TreeRunName", nullptr, flags)) {
			ImGui::Text("積極性 : %f", _aggressionScore);
			ImGui::Text("行動 : %s", root_->GetCurrentRunNodeName().c_str());
			switch (rootState_) {
			case Inactive:
				ImGui::Text("状態 : 待機");
				break;
			case Success:
				ImGui::Text("状態 : 成功");
				break;
			case Failure:
				ImGui::Text("状態 : 失敗");
				break;
			case Running:
				ImGui::Text("状態 : 実行中");
				break;
			default:
				break;
			}
		}
		ImGui::End();
	}
	ImGui::End();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　編集処理
//////////////////////////////////////////////////////////////////////////////////////////////////

void BehaviorTree::Edit() {
	if (!isOpenEditor_) return;
	if (ImGui::Begin(GetName().c_str(), &isOpenEditor_)) {
		// Treeに関する処理
		ImGui::Text("isExecute");
		ImGui::SameLine();
		ImGui::Checkbox("##isExecute", &isExecute_);
		ImGui::SameLine();
		if (ImGui::Button("OverWirte")) {
			json data = root_->ToJson();
			editor_.CommentsToJson(data);
			BehaviorTreeSerializer::Save(fileDirectory_ + fileName_, data);
		}
		ImGui::SameLine();
		if (ImGui::Button("ReSet")) {
			CreateTree(fileDirectory_, fileName_);
		}
		ImGui::SameLine();
		editor_.Edit(name_, nodeList_, links_, root_, blackboard_, creators_, goalArray_);

		if (ImGui::Begin("Blackboard")) {
			if (blackboard_) {
				blackboard_->Debug_Gui();
			}
		}
		ImGui::End();

		if (ImGui::Begin("WeightTable")) {
			if (ImGui::BeginTable("WeightTable", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
				ImGui::TableSetupColumn("name");
				ImGui::TableSetupColumn("重み");
				ImGui::TableSetupColumn("積極性");
				ImGui::TableHeadersRow();

				for (auto it = nodeList_.begin(); it != nodeList_.end();) {
					BaseBehaviorNode* node = (*it).get();
					if (node->GetNodeType() == NodeType::Task) {
						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);
						ImGui::Text(node->GetName().c_str());
						ImGui::TableSetColumnIndex(1);
						node->WeightTableItem();
					}
					it++;
				}
				ImGui::EndTable();
			}
		}
		ImGui::End();
	}

	ImGui::End();
}

void BehaviorTree::EditSelect() {
	editor_.EditSelect();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// ↓　Treeを作成
//////////////////////////////////////////////////////////////////////////////////////////////////

void BehaviorTree::CreateTree(const std::string& fileDirectory, const std::string& fileName) {
	nodeList_.clear();
	if (root_ != nullptr) {
		root_->ClearChild();
	}

	std::string fullPath = fileDirectory + fileName;

	if (!std::filesystem::exists(fullPath)) {
		BehaviorTreeSerializer::Save(fullPath, json());
	}

	if (fullPath == "") {
		return;
	}

	// treeの生成前にloggerを作成しておく
	logger_ = std::make_unique<BehaviorTreeLogger>();
	logger_->Init(fullPath);
	fileDirectory_ = fileDirectory;
	fileName_ = fileName;

	// jsonからtreeの情報を読み取る
	editor_.Init(fileDirectory_, fileName_);
	json nodeTree = BehaviorTreeSerializer::LoadToJson(fullPath);
	root_ = BehaviorTreeNodeFactory::CreateNodeFromJson(nodeTree, nodeList_, links_, blackboard_, creators_, goalArray_);
	editor_.CreateCommentsFromJson(nodeTree);

	std::string message = fullPath + "を作成しました。";
	AOENGINE::Logger::Log(message);
}
