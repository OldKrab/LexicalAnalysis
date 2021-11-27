#pragma once
#include <iostream>

#include "Node.h"
#include "Types.h"

struct EmptyNode:  Node
{
	EmptyNode(Node* parent):Node("", parent){}

	DataType GetDataType() const override
	{
		return DataType::Unknown;
	}
	SemanticType GetSemanticType() const override { return SemanticType::Empty; }

protected:
	void Print(std::ostream& out) const override
	{
		out << "Empty Node\n";
	}
};
