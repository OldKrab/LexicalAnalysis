#include "SemanticTree.h"
#include "Types/DataType.h"
#include "Types/LexemeType.h"
#include "Types/SemanticType.h"
#include "Node/FuncData.h"
#include "Node/VarData.h"
#include <memory>

#include "Exceptions/AnalysisExceptions.h"

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
	if (value.type != type)
		std::cout << "Cast " << value;
	else
		return value;
	switch (type)
	{
	case DataType::Int:
		if (value.type == DataType::Long)
			value.intVal = static_cast<int>(value.longVal);
		break;
	case DataType::Long:
		if (value.type == DataType::Int)
			value.longVal = value.intVal;
		break;
	default: break;
	}

	value.type = type;

	std::cout << " to " << value << "\n";
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
	std::cout << "Perform operation: " << leftValue << " " << LexemeTypeToString(operation) << " " << rightValue << "\n";

	auto resType = GetResultDataType(leftValue.type, rightValue.type, operation);
	CastOperands(leftValue, rightValue, operation);

	DataValue resValue = DataValue(resType);
	if (resType == DataType::Long)
		switch (operation) {
		case LexemeType::Plus:
			resValue.longVal = leftValue.longVal + rightValue.longVal;
			break;
		case LexemeType::Minus:
			resValue.longVal = leftValue.longVal - rightValue.longVal;
			break;
		case LexemeType::Mul:
			resValue.longVal = leftValue.longVal * rightValue.longVal;
			break;
		case LexemeType::Div:
			resValue.longVal = leftValue.longVal / rightValue.longVal;
			break;
		case LexemeType::Modul:
			resValue.longVal = leftValue.longVal % rightValue.longVal;
			break;
		default:
			throw std::exception("not known operation");
		}
	else
	{
		switch (operation) {

		case LexemeType::E:
			resValue.intVal = leftValue.type == DataType::Int ? leftValue.intVal == rightValue.intVal : leftValue.longVal == rightValue.longVal;
			break;
		case LexemeType::NE:
			resValue.intVal = leftValue.type == DataType::Int ? leftValue.intVal != rightValue.intVal : leftValue.longVal != rightValue.longVal;
			break;
		case LexemeType::G:
			resValue.intVal = leftValue.type == DataType::Int ? leftValue.intVal > rightValue.intVal : leftValue.longVal > rightValue.longVal;
			break;
		case LexemeType::L:
			resValue.intVal = leftValue.type == DataType::Int ? leftValue.intVal < rightValue.intVal : leftValue.longVal < rightValue.longVal;
			break;
		case LexemeType::LE:
			resValue.intVal = leftValue.type == DataType::Int ? leftValue.intVal <= rightValue.intVal : leftValue.longVal <= rightValue.longVal;
			break;
		case LexemeType::GE:
			resValue.intVal = leftValue.type == DataType::Int ? leftValue.intVal >= rightValue.intVal : leftValue.longVal >= rightValue.longVal;
			break;
		case LexemeType::Plus:
			resValue.intVal = leftValue.intVal + rightValue.intVal;
			break;
		case LexemeType::Minus:
			resValue.intVal = leftValue.intVal - rightValue.intVal;
			break;
		case LexemeType::Mul:
			resValue.intVal = leftValue.intVal * rightValue.intVal;
			break;
		case LexemeType::Div:
			resValue.intVal = leftValue.intVal / rightValue.intVal;
			break;
		case LexemeType::Modul:
			resValue.intVal = leftValue.intVal % rightValue.intVal;
			break;
		default:
			throw std::exception("not known operation");
		}
	}

	std::cout << "Result of operation:" << resValue << "\n\n";

	return resValue;
}

DataValue SemanticTree::PerformPrefixOperation(LexemeType operation, DataValue value)
{
	std::cout << "Perform operation: " << LexemeTypeToString(operation) << " " << value << "\n";

	DataValue resValue = DataValue(value.type);
	if (value.type == DataType::Long)
		switch (operation) {
		case LexemeType::Plus:
			resValue.longVal = +value.longVal;
			break;
		case LexemeType::Minus:
			resValue.longVal = -value.longVal;
			break;
		case LexemeType::Inc:
			resValue.longVal = ++value.longVal;
			break;
		case LexemeType::Dec:
			resValue.longVal = --value.longVal;
			break;
		}
	else
	{
		switch (operation) {
		case LexemeType::Plus:
			resValue.intVal = +value.intVal;
			break;
		case LexemeType::Minus:
			resValue.intVal = -value.intVal;
			break;
		case LexemeType::Inc:
			resValue.intVal = ++value.intVal;
			break;
		case LexemeType::Dec:
			resValue.intVal = --value.intVal;
			break;
		}
	}

	std::cout << "Result of operation:" << resValue << "\n\n";

	return resValue;
}


DataValue SemanticTree::GetValueOfNum(Lexeme lex)
{
	auto type = GetDataTypeOfNum(lex);
	switch (type)
	{
	case DataType::Int:
		return DataValue(std::stoi(lex.str, nullptr, 0));
	case DataType::Long:
		return DataValue(std::stoll(lex.str, nullptr, 0));
	default:
		return DataValue(DataType::Unknown);
	}
}

void SemanticTree::SetVariableValue(Node* node, DataValue value)
{
	std::cout << "Set value " << value << " to " << *node;

	auto varData = dynamic_cast<VarData*>(node->Data.get());
	value = CastValue(value, node->GetDataType());
	varData->Value = value;
	SetVariableInitialized(node);

	std::cout << "Node after set: " << *node << "\n";
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


void SemanticTree::CastOperands(DataValue& leftValue, DataValue& rightValue, LexemeType operation)
{
	if (rightValue.type == DataType::Void || leftValue.type == DataType::Void
		|| rightValue.type == DataType::Unknown || leftValue.type == DataType::Unknown)
		return;

	if (leftValue.type == DataType::Long || rightValue.type == DataType::Long)
	{
		leftValue = CastValue(leftValue, DataType::Long);
		rightValue = CastValue(rightValue, DataType::Long);
	}
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

