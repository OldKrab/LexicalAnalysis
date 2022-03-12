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
	if (Child)
		Child->RecursivePrint(out, tabCount + 1);
	if (Siblink)
		Siblink->RecursivePrint(out, tabCount);
}

std::unique_ptr<Node> Node::Clone(Node* parent) const
{
	if (Data) return std::make_unique<Node>(parent, Data->Clone());
	return std::make_unique<Node>(parent);
}




