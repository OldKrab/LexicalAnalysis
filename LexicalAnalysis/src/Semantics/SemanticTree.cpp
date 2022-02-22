#include "SemanticTree.h"
#include "Types/DataType.h"
#include "Types/LexemeType.h"
#include "Types/SemanticType.h"
#include "Node/FuncData.h"
#include "Node/VarData.h"
#include <memory>

SemanticTree::SemanticTree()
	:_rootNode(std::make_unique<Node>(nullptr)),
	_currNode(_rootNode.get())
{}

void SemanticTree::SetCurrentNode(Node* node)
{
	_currNode = node;
}

Node* SemanticTree::AddVariable(DataType type, const std::string& id)
{
	_currNode->LeftChild = std::make_unique<Node>(_currNode);
	_currNode->LeftChild->Data = std::make_unique<VarData>(id, type);
	SetCurrentNode(_currNode->LeftChild.get());
	return _currNode;
}

DataValue SemanticTree::GetVariableValue(Node* node)
{
	return dynamic_cast<VarData*>(node->Data.get())->Value;
}

DataValue SemanticTree::CastValue(DataValue value, DataType type)
{
	switch (type)
	{
	case DataType::Long:
		if (value.type == DataType::Int)
			value.intVal = static_cast<int>(value.longVal);
		break;
	case DataType::Int:
		if (value.type == DataType::Int)
			value.longVal = value.intVal;
		break;
	default: break;
	}
	if(value.type != type)
	{
		std::cout << "Cast from " << DataTypeToString(value.type) << " to " << DataTypeToString(type) << "\n";
	}
	value.type = type;
	return value;
}

void SemanticTree::SetVariableInitialized(Node* varNode)
{
	dynamic_cast<VarData*>(varNode->Data.get())->IsInitialized = true;
}

bool SemanticTree::GetVariableInitialized(Node* varNode)
{
	return dynamic_cast<VarData*>(varNode->Data.get())->IsInitialized;
}

bool SemanticTree::CheckUniqueIdentifier(const std::string& id) const
{
	auto node = FindNodeUpInScope(id);
	return node->GetSemanticType() == SemanticType::Empty;
}


DataValue SemanticTree::PerformOperation(DataValue leftValue, DataValue rightValue, LexemeType operation)
{
	// cast before
	throw std::exception("Not implemented");
}

DataValue SemanticTree::PerformPrefixOperation(LexemeType operation, DataValue value)
{
	throw std::exception("Not implemented");
}

DataValue SemanticTree::PerformPostfixOperation(DataValue value, LexemeType operation)
{
	throw std::exception("Not implemented");
}

DataValue SemanticTree::GetValueOfNum(Lexeme lex)
{
	auto type = GetDataTypeOfNum(lex);
	switch (type)
	{
	case DataType::Int:
		return DataValue(std::stoi(lex.str,nullptr,0));
	case DataType::Long:
		return DataValue(std::stoll(lex.str,nullptr,0));
	default:
		return DataValue(DataType::Unknown);
	}
}

void SemanticTree::SetVariableValue(Node* node, DataValue value)
{
	std::cout << "Node before:\n";
	node->Print(std::cout);

	auto varData = dynamic_cast<VarData*>(node->Data.get());
	value = CastValue(value, node->GetDataType());
	varData->Value = value;
	SetVariableInitialized(node);

	std::cout << "Node after:\n";
	node->Print(std::cout);
	std::cout << "\n";
}

DataType SemanticTree::GetResultDataType(DataType leftType, DataType rightType, LexemeType operation)
{
	if (leftType == DataType::Void || rightType == DataType::Void
		|| leftType == DataType::Unknown || rightType == DataType::Unknown)
		return DataType::Unknown;

	if (operation == LexemeType::E || operation == LexemeType::NE
		|| operation == LexemeType::G || operation == LexemeType::GE
		|| operation == LexemeType::L || operation == LexemeType::LE)
		return DataType::Int;

	if (leftType == DataType::Long || rightType == DataType::Long)
		return DataType::Long;
	return DataType::Int;
}

DataType SemanticTree::GetResultDataType(DataType type, LexemeType operation)
{
	if (type == DataType::Void || type == DataType::Unknown)
		return DataType::Unknown;
	return type;
}

DataType SemanticTree::GetDataTypeOfNum(Lexeme lex)
{
	static std::string MAX_INT = "2147483647";
	static std::string MAX_INT_H = "7FFFFFFF";
	static std::string MAX_INT_O = "017777777777";
	static std::string MAX_LONG = "9223372036854775807";
	static std::string MAX_LONG_H = "7FFFFFFFFFFFFFFF";
	static std::string MAX_LONG_O = "0777777777777777777777";
	if (lex.str.back() == 'l' || lex.str.back() == 'L')
		return DataType::Long;
	if (lex.type == LexemeType::DecimNum)
	{
		if (lex.str.size() < MAX_INT.size() || lex.str <= MAX_INT)
			return DataType::Int;
		if (lex.str.size() < MAX_LONG.size() || lex.str <= MAX_LONG)
			return DataType::Long;
		return  DataType::Unknown;
	}
	if (lex.type == LexemeType::OctNum)
	{
		if (lex.str.size() < MAX_INT_O.size() || lex.str <= MAX_INT_O)
			return DataType::Int;
		if (lex.str.size() < MAX_LONG_O.size() || lex.str <= MAX_LONG_O)
			return DataType::Long;
		return  DataType::Unknown;
	}
	if (lex.type == LexemeType::HexNum)
	{
		auto suff = lex.str.substr(2);
		if (suff.size() < MAX_INT_H.size() || suff <= MAX_INT_H)
			return DataType::Int;
		if (suff.size() < MAX_LONG_H.size() || suff <= MAX_LONG_H)
			return DataType::Long;
		return  DataType::Unknown;
	}
	return DataType::Unknown;
}

bool SemanticTree::CheckDefinedIdentifier(const std::string& id) const
{
	auto node = FindNodeUp(id);
	return node->GetSemanticType() != SemanticType::Empty;
}

bool SemanticTree::CheckCastable(DataType from, DataType to)
{
	if (from == DataType::Void || to == DataType::Void
		|| from == DataType::Unknown || to == DataType::Unknown)
		return false;
	return true;
}

Node* SemanticTree::AddFunc(const std::string& id)
{
	_currNode->LeftChild = std::make_unique<Node>(_currNode);
	_currNode->LeftChild->Data = std::make_unique<FuncData>(id);
	const auto funcNode = _currNode->LeftChild.get();
	SetCurrentNode(funcNode);
	AddScope();
	return funcNode;
}

Node* SemanticTree::AddEmpty()
{
	_currNode->LeftChild = std::make_unique<Node>(_currNode);
	SetCurrentNode(_currNode->LeftChild.get());
	return _currNode;
}

void SemanticTree::AddParam(Node* funcNode, const std::string& id, DataType type)
{
	auto funcData = dynamic_cast<FuncData*>(funcNode->Data.get());
	funcData->ParamsCount++;
	auto node = AddVariable(type, id);
	SetVariableInitialized(node);
}

std::vector<DataType> SemanticTree::GetFuncParams(Node* funcNode)
{
	const auto funcData = dynamic_cast<FuncData*>(funcNode->Data.get());
	auto paramNode = funcNode->RightChild->LeftChild.get();
	std::vector<DataType> paramsTypes(funcData->ParamsCount);
	for (int i = 0; i < funcData->ParamsCount; i++)
	{
		paramsTypes[i] = paramNode->GetDataType();
		paramNode = paramNode->LeftChild.get();
	}
	return paramsTypes;
}

void SemanticTree::AddScope()
{
	_currNode->RightChild = std::make_unique<Node>(_currNode);
	SetCurrentNode(_currNode->RightChild.get());
}

void SemanticTree::Print(std::ostream& out) const
{
	_rootNode->RecursivePrint(out);
}

Node* SemanticTree::FindNodeUp(const std::string& id) const
{
	auto node = _currNode;
	while (node->Parent && (node->Data == nullptr || node->Data->Identifier != id)) {
		node = node->Parent;
	}
	return node;
}

Node* SemanticTree::FindNodeUpInScope(const std::string& id) const
{
	auto node = _currNode;
	auto par = _currNode->Parent;
	while (par && par->RightChild.get() != node && (node->Data == nullptr || node->Data->Identifier != id)) {
		node = par;
		par = node->Parent;

	}
	return node;
}
