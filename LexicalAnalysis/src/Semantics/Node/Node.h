#pragma once
#include <memory>
#include <string>
#include <utility>

enum class NodeType
{
	Func, Var, Empty
};

struct Node
{
	Node(std::string id) :Identifier(std::move(id)) {}

	virtual void RecPrint(std::ostream& out, int tabCount = 0) const
	{
		std::string tab("\t", tabCount);
		out << tab;
		Print(out);
		if (LeftChild)
			LeftChild->RecPrint(out, tabCount);
		if (RightChild)
			RightChild->RecPrint(out, tabCount + 1);
	}

	virtual NodeType GetType() const = 0;

	Node* Parent = nullptr;
	std::unique_ptr<Node> LeftChild, RightChild;
	std::string Identifier;

protected:
	virtual void Print(std::ostream& out) const = 0;
};
