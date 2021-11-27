#pragma once
#include <memory>
#include <string>
#include <utility>

struct Node
{
	Node(std::string id, Node* parent) :Parent(parent), Identifier(std::move(id)) {}
	virtual ~Node() = default;

	virtual void RecPrint(std::ostream& out, int tabCount = 0) const
	{
		std::string tab(tabCount, '\t');
		out << tab;
		Print(out);
		if (RightChild)
			RightChild->RecPrint(out, tabCount + 1);
		if (LeftChild)
			LeftChild->RecPrint(out, tabCount);
		
	}

	virtual DataType GetDataType() const = 0;
	virtual SemanticType GetSemanticType() const = 0;

	Node* Parent = nullptr;
	std::unique_ptr<Node> LeftChild, RightChild;
	std::string Identifier;

protected:
	virtual void Print(std::ostream& out) const = 0;
};
