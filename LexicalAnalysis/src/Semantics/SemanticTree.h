#pragma once
#include <iostream>
#include <memory>
#include <vector>

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

	Node* GetCurrentNode() const;
	void SetCurrentNode(Node* node);

	Node* AddVariable(DataType type, const std::string& id);
	static void AssignVariable(const Node* node, DataType type);
	static void CheckInitialized(const Node* node);
	static DataType PerformOperation(DataType leftType, DataType rightType, LexemeType operation);
	static DataType PerformPrefixOperation(LexemeType operation, DataType type);

	static void CheckValidFuncArgs(const Node* funcNode, const std::vector<DataType>& args);

	Node* AddFunction(const std::string& id);
	void AddParam(const Node* funcNode, const std::string& id, DataType type);
	static void SetFunctionPos(const Node* funcNode, const SourceText::Iterator& pos);
	static SourceText::Iterator GetFunctionPos(const Node* funcNode);
	Node* CloneFunctionDefinition(Node* origNode) const;
	void DeleteFuncDefinition(Node* funcNode) const;

	Node* AddEmpty();
	void AddScope();

	Node* FindVariableNodeUp(const std::string& id) const;
	Node* FindFunctionNodeUp(const std::string& id) const;

	static void DeleteSubTree(Node* node);

	void Print(std::ostream& out = std::cout) const;
	static void CheckCastable(DataType from, DataType to);
	static DataType GetDataTypeOfNum(Lexeme lex);

private:
	bool CheckUniqueIdentifier(const std::string& id) const;
	static void CheckOperationValid(DataType leftType, DataType rightType, LexemeType operation);
	static void CheckOperationValid(DataType type, LexemeType operation);

	Node* FindNodeUpInScope(const std::string& id) const;
	Node* FindNodeUp(const std::string& id) const;
	static std::vector<DataType> GetFunctionParams(const Node* funcNode);

	static bool GetVariableInitialized(const Node* varNode);
	static void SetVariableInitialized(const Node* varNode);

	static VarData* GetVariableData(const Node* node);

	static DataType GetResultDataType(DataType leftType, DataType rightType, LexemeType operation);

	static FuncData* GetFunctionData(const Node* funcNode);


	std::unique_ptr<Node> _rootNode;
	Node* _currNode;
};



