#pragma once
#include <iostream>
#include <memory>
#include <vector>

#include "Interpretator/DataValue.h"
#include "Lexical/Lexeme.h"
#include "Node/Node.h"
#include "Types/DataType.h"
#include "Types/LexemeType.h"

class SemanticTree
{
public:
	SemanticTree();

	void SetCurrentNode(Node* node);

	Node* AddVariable(DataType type, const std::string& id);
	static void SetVariableInitialized(Node* varNode);
	static bool GetVariableInitialized(Node* varNode);
	bool CheckUniqueIdentifier(const std::string& id) const;
	bool CheckDefinedIdentifier(const std::string& id) const;
	static bool CheckCastable(DataType from, DataType to);

	static DataValue GetVariableValue(Node* node);
	static DataValue CastValue(DataValue value, DataType type);
	static DataValue PerformOperation(DataValue leftValue, DataValue rightValue, LexemeType operation);
	static DataValue PerformPrefixOperation(LexemeType operation, DataValue value);
	static DataValue GetValueOfNum(Lexeme lex);
	static void SetVariableValue(Node* node, DataValue value);
	static DataType GetResultDataType(DataType leftType, DataType rightType, LexemeType operation);
	static void CastOperands(DataValue& leftValue, DataValue& rightValue, LexemeType operation);
	static DataType GetDataTypeOfNum(Lexeme lex);

	Node* AddFunc(const std::string& id);
	void AddParam(Node* funcNode, const std::string& id, DataType type);
	static std::vector<DataType> GetFuncParams(Node* funcNode);

	Node* AddEmpty();

	void AddScope();
	void Print(std::ostream& out = std::cout) const;

	Node* FindNodeUp(const std::string& id) const;
	Node* FindNodeUpInScope(const std::string& id) const;

private:
	std::unique_ptr<Node> _rootNode;
	Node* _currNode;
};



