#pragma once
#include "LexemeType.h"
#include "Node.h"
#include <iostream>
#include <utility>


struct VarNode : Node
{
	VarNode(std::string id, LexemeType Type)
	: Node(std::move(id)), Type(Type){}

	NodeType GetType() const override { return NodeType::Var; }

	LexemeType Type;
	bool IsInitialized = false;
protected:
	void Print(std::ostream& out) const override
	{
		out << "Variable Node: Type = " << LexemeTypeToString(Type) << ", Id = " << Identifier << ", Is Initialized = " << IsInitialized << "\n";
	}
};
