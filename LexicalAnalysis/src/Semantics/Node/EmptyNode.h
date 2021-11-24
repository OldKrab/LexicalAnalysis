#pragma once
#include <iostream>

#include "Node.h"

struct EmptyNode:  Node
{
	EmptyNode():Node(""){}

	NodeType GetType() const override
	{
		return NodeType::Empty;
	}

protected:
	void Print(std::ostream& out) const override
	{
		out << "Empty Node\n";
	}
};
