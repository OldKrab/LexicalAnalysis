#include "SemanticTree.h"

#include "Node/EmptyNode.h"
#include "Node/VarNode.h"
#include "Node/FuncNode.h"

SemanticTree::SemanticTree()
	:_rootNode(std::make_unique<EmptyNode>()),
	_currNode(_rootNode.get())
{}

void SemanticTree::SetCurrentNode(Node* node)
{
	_currNode = node;
}

void SemanticTree::AddVariable(const std::string& id, LexemeType type)
{
	_currNode->LeftChild = std::make_unique<VarNode>(id, type);
	SetCurrentNode(_currNode->LeftChild.get());
}

Node* SemanticTree::AddFunc(const std::string& id)
{
	_currNode->LeftChild = std::make_unique<FuncNode>(id);
	const auto funcNode = _currNode->LeftChild.get();
	AddScope();
	return funcNode;
}

void SemanticTree::AddParam(Node* funcNode, const std::string& id, LexemeType type)
{
	auto func = dynamic_cast<FuncNode*>(funcNode);
	func->ParamsCount++;
	AddVariable(id, type);
}

void SemanticTree::AddScope()
{
	_currNode->RightChild = std::make_unique<EmptyNode>();
	SetCurrentNode(_currNode->RightChild.get());
}

void SemanticTree::Print(std::ostream& out) const
{
	_rootNode->RecPrint(out);
}
