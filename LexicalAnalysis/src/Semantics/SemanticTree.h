#pragma once
#include <memory>

#include "LexemeType.h"
#include "Node/Node.h"

class SemanticTree
{
public:
	SemanticTree();

	void SetCurrentNode(Node* node);

	void AddVariable(const std::string& id, LexemeType type);
	Node* AddFunc(const std::string& id);
	void AddParam(Node* funcNode, const std::string& id, LexemeType type);
	void AddScope();
	void Print(std::ostream& out) const;
private:
	Node* FindUp(const std::string& id) const;

	std::unique_ptr<Node> _rootNode;
	Node* _currNode;
};

