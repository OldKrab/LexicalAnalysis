#pragma once
#include <memory>
#include <string>
#include "NodeData.h"
#include "Types/DataType.h"
#include "Types/SemanticType.h"

struct Node
{
	Node(Node* parent) :Parent(parent) {}
	Node(Node* parent,std::unique_ptr<NodeData> data) :Parent(parent), Data(std::move(data)) {}

	void Print(std::ostream& out, int tabCount = 0) const;
	void RecursivePrint(std::ostream& out, int tabCount = 0) const;

	DataType GetDataType() const { return Data ? Data->GetDataType() : DataType::Unknown; }

	SemanticType GetSemanticType() const { return Data ? Data->GetSemanticType() : SemanticType::Empty; }

	std::unique_ptr<Node> Clone(Node* parent) const;

	Node* Parent = nullptr;
	std::unique_ptr<Node> Siblink, Child;
	std::unique_ptr<NodeData> Data;

};

std::ostream& operator<<(std::ostream& out, const Node& node);
