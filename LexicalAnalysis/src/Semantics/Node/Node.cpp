#include "Node.h"
#include <iostream>

void Node::Print(std::ostream& out, int tabCount) const
{
	std::string tab(tabCount, '\t');
	out << tab;
	if (Data)
		Data->Print(out);
	else
		out << "()\n";
}

std::ostream& operator<<(std::ostream& out, const Node& node)
{
	node.Print(out, 0);
	return out;
}

void Node::RecursivePrint(std::ostream& out, int tabCount) const
{
	Print(out, tabCount);
	if (RightChild)
		RightChild->RecursivePrint(out, tabCount + 1);
	if (LeftChild)
		LeftChild->RecursivePrint(out, tabCount);
}

std::unique_ptr<Node> Node::CloneRecursive() const
{
	auto node = std::make_unique<Node>(Parent);
	node->Data = Data->Clone();
	if (LeftChild) node->LeftChild = LeftChild->CloneRecursive();
	if (RightChild) node->RightChild = RightChild->CloneRecursive();
	return node;
}


