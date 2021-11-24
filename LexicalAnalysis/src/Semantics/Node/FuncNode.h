#pragma once
#include <iostream>
#include <utility>

#include "LexemeType.h"
#include "Node.h"


struct FuncNode : Node
{
	FuncNode(std::string id):Node(std::move(id)){}

	NodeType GetType() const override { return NodeType::Func; }

	int ParamsCount = 0;
protected:
	void Print(std::ostream& out) const override
	{
		out << "Function Node: Id = " << Identifier << "\n";
	}
};
