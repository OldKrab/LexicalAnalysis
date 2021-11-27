#pragma once
#include <iostream>
#include <utility>

#include "Types.h"
#include "Node.h"


struct FuncNode : Node
{
	FuncNode(Node* parent, std::string id) :Node(std::move(id), parent) {}

	DataType GetDataType() const override { return DataType::Void; }

	SemanticType GetSemanticType() const override { return SemanticType::Func; }

	int ParamsCount = 0;

protected:
	void Print(std::ostream& out) const override
	{
		out << "Function Node: Id = " << Identifier << ", Param Count = " << ParamsCount << "\n";
	}
};
