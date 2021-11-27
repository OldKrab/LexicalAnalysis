#pragma once
#include "Types.h"
#include "Node.h"
#include <iostream>
#include <utility>


struct VarNode : Node
{
	VarNode(Node* parent, std::string id, DataType type, bool isInitialized)
		: Node(std::move(id), parent),
		Type(type),
		IsInitialized(isInitialized) {}

	DataType GetDataType() const override { return Type; }
	SemanticType GetSemanticType() const override { return SemanticType::Var; }

	DataType Type;
	bool IsInitialized;

protected:
	void Print(std::ostream& out) const override
	{
		out << "Variable Node: Type = " << DataTypeToString(Type) << ", Id = " << Identifier << ", Is Initialized = " << IsInitialized << "\n";
	}
};
