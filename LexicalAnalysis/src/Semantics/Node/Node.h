#pragma once
#include <memory>
#include <string>
#include "NodeData.h"
#include "Types/DataType.h"
#include "Types/SemanticType.h"

struct Node
{
	Node(Node* parent) :Parent(parent) {}

	void Print(std::ostream& out, int tabCount = 0) const;
	void RecursivePrint(std::ostream& out, int tabCount = 0) const;

	std::unique_ptr<Node> CloneRecursive() const;

	DataType GetDataType() const { return Data ? Data->GetDataType() : DataType::Unknown; }

	SemanticType GetSemanticType() const { return Data ? Data->GetSemanticType() : SemanticType::Empty; }

	Node* Parent = nullptr;
	std::unique_ptr<Node> LeftChild, RightChild;
	std::unique_ptr<NodeData> Data;

};

std::ostream& operator<<(std::ostream& out, const Node& node);
