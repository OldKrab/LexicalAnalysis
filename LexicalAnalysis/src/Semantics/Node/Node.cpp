#include "Node.h"
#include <iostream>

void Node::RecursivePrint(std::ostream& out, int tabCount) const
{
	std::string tab(tabCount, '\t');
	out << tab;
	if (Data)
		Data->Print(out);
	else
		out << "()\n";
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
