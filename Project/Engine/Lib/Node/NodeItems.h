#pragma once
#include <string>
#include <vector>
#include "Engine/System/Manager/ImGuiManager.h"
#include "Engine/Lib/Math/Vector2.h"
#include "Engine/Lib/Math/Vector3.h"

namespace AOENGINE {

enum class PinType {
	Flow,
	Bool,
	Int,
	Float,
	Vector2,
	Vector3,
	Color,
	String,
	Object,
	Texture,
	Function,
	Delegate,
};

enum class PinKind{
    Output,
    Input
};

/// <summary>
/// ノードピン情報
/// </summary>
struct Pin {
	ax::NodeEditor::PinId id;
	ax::NodeEditor::PinKind kind;
};

/// <summary>
/// ノード情報
/// </summary>
struct Node {
	ax::NodeEditor::NodeId id;
	Pin inputId;
	Pin outputId;
	std::string name;
};

/// <summary>
/// ノードリンク情報
/// </summary>
struct Link {
	ax::NodeEditor::LinkId id;
	ax::NodeEditor::PinId  from;
	ax::NodeEditor::PinId  to;
};

/// <summary>
/// ノード項目情報
/// </summary>
struct NodeItems {
	std::string nodeName;
	int nodeType;
	Math::Vector2 nodePos;
	std::vector<std::string> childName;
};

namespace Custom {
	/// <summary>
	/// ノードピン情報
	/// </summary>
	struct Pin {
		int32_t id;
		PinKind kind;
		PinType type;
		std::string name;

		Pin(int32_t _id, PinKind _kind, PinType _type, const char* _name) :
			id(_id), kind(_kind), type(_type), name(_name) {
		}
	};

	/// <summary>
	/// ノード情報
	/// </summary>
	struct Node {
		int32_t id;
		std::vector<Pin> inputs;
		std::vector<Pin> outputs;
		std::string name;
		ImVec2 position;
	};

	/// <summary>
	/// ノードリンク情報
	/// </summary>
	struct Link {
		int32_t id;
		int32_t from;
		int32_t to;
	};
}

/// <summary>
/// ノードUVトランスフォーム情報
/// </summary>
struct NodeUVTransform {
	Math::Vector2 scale = CMath::Vector2::UNIT;
	Math::Vector2 translate = CMath::Vector2::ZERO;
	float rotate = 0;
};

}

using AOENGINE::PinType;
using AOENGINE::PinKind;
using AOENGINE::Pin;
using AOENGINE::Node;
using AOENGINE::Link;
using AOENGINE::NodeItems;
using AOENGINE::NodeUVTransform;


#ifndef AOENGINE_CUSTOM_NAMESPACE_ALIAS
#define AOENGINE_CUSTOM_NAMESPACE_ALIAS
namespace Custom = AOENGINE::Custom;
#endif
