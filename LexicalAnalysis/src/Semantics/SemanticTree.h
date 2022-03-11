#pragma once
#include <iostream>
#include <memory>
#include <vector>

#include "Interpretator/DataValue.h"
#include "Lexical/Lexeme.h"
#include "Node/FuncData.h"
#include "Node/Node.h"
#include "Node/VarData.h"
#include "Types/DataType.h"
#include "Types/LexemeType.h"

class SemanticTree
{
public:
	SemanticTree();

	Node*  GetCurrentNode() const;
	void SetCurrentNode(Node* node);

	Node* AddVariable(DataType type, const std::string& id);
	DataValue GetVariableValue(const Node* node) const;
	DataValue CastValue(DataValue value, DataType type) const;
	DataValue PerformOperation(DataValue leftValue, DataValue rightValue, LexemeType operation) const;
	DataValue PerformPrefixOperation(LexemeType operation, DataValue value) const;
	DataValue GetValueOfNum(Lexeme lex);
	void SetVariableValue(Node* node, DataValue value) const;
	void CastOperands(DataValue& leftValue, DataValue& rightValue, LexemeType operation) const;
	void CheckOperationValid(DataValue leftValue, DataValue rightValue, const Lexeme& lex) const;
	void CheckOperationValid(DataType type, const Lexeme& lex) const;
	void CheckValidFuncArgs(const Node* funcNode, std::vector<DataValue> args) const;

	Node* AddFunction(const std::string& id);
	void AddParam(Node* funcNode, const std::string& id, DataType type);
	void SetFunctionPos(const Node* funcNode, const SourceText::Iterator& pos) const;
	SourceText::Iterator GetFunctionPos(const Node* funcNode) const;
	Node* CloneFunctionDefinition( Node* node) const;
	 Node* GetFunctionBodyNode( Node* funcNode) const;
	void DeleteFuncDefinition( Node* funcNode) const;

	Node* AddEmpty();
	void AddScope();

	Node* FindVariableNodeUp(const std::string& id) const;
	Node* FindFunctionNodeUp(const std::string& id) const;

	void DeleteSubTree(Node* node);

	void Print(std::ostream& out = std::cout) const;

	bool IsInterpretation = true;
private:
	bool CheckUniqueIdentifier(const std::string& id) const;
	static void CheckCastable(DataType from, DataType to);

	Node* FindNodeUpInScope(const std::string& id) const;
	Node* FindNodeUp(const std::string& id) const;
	std::vector<DataType> GetFunctionParams(const Node* funcNode) const;

	static bool GetVariableInitialized(const Node* varNode);
	static void SetVariableInitialized(const Node* varNode);

	static VarData* GetVariableData(const Node* node);

	static DataType GetResultDataType(DataType leftType, DataType rightType, LexemeType operation);
	static DataType GetDataTypeOfNum(Lexeme lex);

	static FuncData* GetFunctionData(const Node* funcNode);


	std::unique_ptr<Node> _rootNode;
	Node* _currNode;
};



