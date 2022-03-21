#include "SemanticTree.h"
#include "Types/DataType.h"
#include "Types/LexemeType.h"
#include "Types/SemanticType.h"
#include "Node/FuncData.h"
#include "Node/VarData.h"
#include <memory>

#include "Exceptions/AnalysisExceptions.h"

using std::unique_ptr;
using std::shared_ptr;
using std::make_unique;
using std::make_shared;

SemanticTree::SemanticTree()
	:_rootNode(make_unique<Node>(nullptr)),
	_currNode(_rootNode.get())
{}

Node* SemanticTree::GetCurrentNode() const
{
	if (!IsInterpretation) return nullptr;

	return  _currNode;
}

void SemanticTree::SetCurrentNode(Node* node)
{
	if (!IsInterpretation) return;

	_currNode = node;
}

Node* SemanticTree::AddVariable(DataType type, const std::string& id)
{
	if (!IsInterpretation) return nullptr;

	if (!CheckUniqueIdentifier(id))
		throw RedefinedIdentifierException(id);

	_currNode->Siblink = make_unique<Node>(_currNode, make_unique<VarData>(id, type));
	SetCurrentNode(_currNode->Siblink.get());
	return _currNode;
}

shared_ptr<DataValue> SemanticTree::GetVariableValue(const Node* node) const
{
	if (!IsInterpretation) return {};

	if (!GetVariableInitialized(node))
		throw UsingUninitializedVariableException(node->Data->Identifier);

	return GetVariableData(node)->Value;
}

void SemanticTree::CastValue(DataValue* value, DataType type) const
{
	if (!IsInterpretation || value->type == type) return;

	CheckCastable(value->type, DataType::Int);

	switch (type)
	{
	case DataType::Int:
		if (value->type == DataType::Long)
			value->intVal = static_cast<int>(value->longVal);
		break;
	case DataType::Long:
		if (value->type == DataType::Int)
			value->longVal = value->intVal;
		break;
	default: break;
	}

	value->type = type;
}

shared_ptr<DataValue> SemanticTree::PerformOperation(shared_ptr<DataValue> leftValue, shared_ptr<DataValue> rightValue, LexemeType operation) const
{
	if (!IsInterpretation) return nullptr;

	CheckOperationValid(leftValue, rightValue, operation);

	auto rightCastValue = make_shared<DataValue>(*rightValue);
	auto leftCastValue = make_shared<DataValue>(*leftValue);
	CastOperands(leftCastValue.get(), rightCastValue.get(), operation);
	auto resValue = make_shared<DataValue>();
	auto resType = leftValue->type;

	if (resType == DataType::Long)
		switch (operation) {
		case LexemeType::E:
			resValue->longVal = leftValue->longVal == rightCastValue->longVal; break;
		case LexemeType::NE:
			resValue->longVal = leftValue->longVal != rightCastValue->longVal; break;
		case LexemeType::G:
			resValue->longVal = leftValue->longVal > rightCastValue->longVal; break;
		case LexemeType::L:
			resValue->longVal = leftValue->longVal < rightCastValue->longVal; break;
		case LexemeType::LE:
			resValue->longVal = leftValue->longVal <= rightCastValue->longVal; break;
		case LexemeType::GE:
			resValue->longVal = leftValue->longVal >= rightCastValue->longVal; break;
		case LexemeType::Plus:
			resValue->longVal = leftValue->longVal + rightCastValue->longVal; break;
		case LexemeType::Minus:
			resValue->longVal = leftValue->longVal - rightCastValue->longVal; break;
		case LexemeType::Mul:
			resValue->longVal = leftValue->longVal * rightCastValue->longVal; break;
		case LexemeType::Div:
			resValue->longVal = leftValue->longVal / rightCastValue->longVal; break;
		case LexemeType::Modul:
			resValue->longVal = leftValue->longVal % rightCastValue->longVal; break;
		default:
			throw std::exception("not known operation");
		}
	else
		switch (operation) {
		case LexemeType::E:
			resValue->intVal = leftValue->intVal == rightCastValue->intVal; break;
		case LexemeType::NE:
			resValue->intVal = leftValue->intVal != rightCastValue->intVal; break;
		case LexemeType::G:
			resValue->intVal = leftValue->intVal > rightCastValue->intVal; break;
		case LexemeType::L:
			resValue->intVal = leftValue->intVal < rightCastValue->intVal; break;
		case LexemeType::LE:
			resValue->intVal = leftValue->intVal <= rightCastValue->intVal; break;
		case LexemeType::GE:
			resValue->intVal = leftValue->intVal >= rightCastValue->intVal; break;
		case LexemeType::Plus:
			resValue->intVal = leftValue->intVal + rightCastValue->intVal; break;
		case LexemeType::Minus:
			resValue->intVal = leftValue->intVal - rightCastValue->intVal; break;
		case LexemeType::Mul:
			resValue->intVal = leftValue->intVal * rightCastValue->intVal; break;
		case LexemeType::Div:
			resValue->intVal = leftValue->intVal / rightCastValue->intVal; break;
		case LexemeType::Modul:
			resValue->intVal = leftValue->intVal % rightCastValue->intVal; break;
		default:
			throw std::exception("not known operation");
		}
	resValue->type = resType;
	return resValue;
}

shared_ptr<DataValue> SemanticTree::PerformPrefixOperation(LexemeType operation, shared_ptr<DataValue> value) const
{
	if (!IsInterpretation) return nullptr;

	CheckOperationValid(value, operation);
	shared_ptr<DataValue> resValue;
	if (operation == LexemeType::Inc || operation == LexemeType::Dec)
		resValue = value;
	else
		resValue = make_shared<DataValue>();
	if (value->type == DataType::Long)
		switch (operation) {
		case LexemeType::Plus:
			resValue->longVal = +value->longVal; break;
		case LexemeType::Minus:
			resValue->longVal = -value->longVal; break;
		case LexemeType::Inc:
			++resValue->longVal; break;
		case LexemeType::Dec:
			--resValue->longVal; break;
		default:
			throw std::exception("not known operation");
		}
	else
	{
		switch (operation) {
		case LexemeType::Plus:
			resValue->intVal = +value->intVal; break;
		case LexemeType::Minus:
			resValue->intVal = -value->intVal; break;
		case LexemeType::Inc:
			++resValue->intVal; break;
		case LexemeType::Dec:
			--resValue->intVal; break;
		default:
			throw std::exception("not known operation");
		}
	}
	return resValue;
}

shared_ptr<DataValue> SemanticTree::ConvertNumLexemeToValue(const Lexeme& lex) const
{
	if (!IsInterpretation) return nullptr;

	auto type = GetDataTypeOfNum(lex);
	switch (type)
	{
	case DataType::Int:
		return make_shared<DataValue>(std::stoi(lex.str, nullptr, 0));
	case DataType::Long:
		return make_shared<DataValue>(std::stoll(lex.str, nullptr, 0));
	default:
		throw InvalidNumberException();
	}
}

void SemanticTree::SetVariableValue(const Node* node, const shared_ptr<DataValue>& value) const
{
	if (!IsInterpretation) return;

	auto varData = GetVariableData(node);
	CheckCastable(value->type, varData->Type);

	CastValue(value.get(), node->GetDataType());
	varData->Value = value;
	SetVariableInitialized(node);
}

std::shared_ptr<DataValue> SemanticTree::CloneValue(const std::shared_ptr<DataValue>& value) const
{
	return IsInterpretation ? std::make_shared<DataValue>(*value) : nullptr;
}

void SemanticTree::CheckOperationValid(shared_ptr<DataValue> leftValue, shared_ptr<DataValue> rightValue, LexemeType operation) const
{
	if (!IsInterpretation) return;

	if (leftValue->type == DataType::Void || rightValue->type == DataType::Void
		|| leftValue->type == DataType::Unknown || rightValue->type == DataType::Unknown)
		throw InvalidOperandsException(leftValue->type, rightValue->type, LexemeTypeToString(operation));

	if ((operation == LexemeType::Div || operation == LexemeType::Modul)
		&& (rightValue->type == DataType::Long && rightValue->longVal == 0
			|| rightValue->type == DataType::Int && rightValue->intVal == 0))
		throw DivisionOnZeroException();
}

void SemanticTree::CheckOperationValid(shared_ptr<DataValue> value, LexemeType operation) const
{
	if (!IsInterpretation) return;

	if (value->type == DataType::Unknown || value->type == DataType::Void)
		throw InvalidOperandsException(value->type, LexemeTypeToString(operation));
}

void SemanticTree::CheckValidFuncArgs(const Node* funcNode, const std::vector<shared_ptr<DataValue>>& args) const
{
	if (!IsInterpretation) return;

	auto paramsTypes = GetFunctionParams(funcNode);

	if (args.size() != paramsTypes.size())
		throw WrongArgsCountException(paramsTypes.size(), args.size(), funcNode->Data->Identifier);

	for (size_t i = 0; i < args.size(); i++)
		CheckCastable(args[i]->type, paramsTypes[i]);
}

void SemanticTree::CastOperands(DataValue* leftValue, DataValue* rightValue, LexemeType operation) const
{
	if (rightValue->type == DataType::Void || leftValue->type == DataType::Void
		|| rightValue->type == DataType::Unknown || leftValue->type == DataType::Unknown)
		return;

	if (leftValue->type == DataType::Long || rightValue->type == DataType::Long)
	{
		CastValue(leftValue, DataType::Long);
		CastValue(rightValue, DataType::Long);
	}
}

Node* SemanticTree::AddFunction(const std::string& id)
{
	if (!IsInterpretation) return nullptr;

	if (!CheckUniqueIdentifier(id))			// Check unique id
		throw RedefinedIdentifierException(id);

	_currNode->Siblink = make_unique<Node>(_currNode, make_unique<FuncData>(id));
	const auto funcNode = _currNode->Siblink.get();
	SetCurrentNode(funcNode);
	AddScope();
	return funcNode;
}

Node* SemanticTree::AddEmpty()
{
	if (!IsInterpretation) return nullptr;

	_currNode->Siblink = make_unique<Node>(_currNode);
	SetCurrentNode(_currNode->Siblink.get());
	return _currNode;
}

void SemanticTree::AddParam(const Node* funcNode, const std::string& id, DataType type)
{
	if (!IsInterpretation) return;

	auto funcData = GetFunctionData(funcNode);
	funcData->ParamsCount++;
	auto node = AddVariable(type, id);
	SetVariableInitialized(node);
}



void SemanticTree::SetFunctionPos(const Node* funcNode, const SourceText::Iterator& pos) const
{
	if (!IsInterpretation) return;
	GetFunctionData(funcNode)->Pos = pos;
}

SourceText::Iterator SemanticTree::GetFunctionPos(const Node* funcNode) const
{
	if (!IsInterpretation) return {};
	return GetFunctionData(funcNode)->Pos;
}



Node* SemanticTree::CloneFunctionDefinition(Node* origNode) const
{
	if (!IsInterpretation) return nullptr;

	auto siblink = std::move(origNode->Siblink);		// swap ownerships of siblinks
	origNode->Siblink = origNode->Clone(origNode);

	auto cloneFuncNode = origNode->Siblink.get();
	auto cloneNode = cloneFuncNode;

	cloneNode->Siblink = std::move(siblink);
	if (cloneNode->Siblink != nullptr)
		cloneNode->Siblink->Parent = cloneNode;
	cloneNode->Child = origNode->Child->Clone(cloneNode);

	origNode = origNode->Child.get();
	cloneNode = cloneNode->Child.get();

	while (origNode->Siblink && origNode->Siblink->GetSemanticType() == SemanticType::Var)
	{
		cloneNode->Siblink = origNode->Siblink->Clone(cloneNode);
		origNode = origNode->Siblink.get();
		cloneNode = cloneNode->Siblink.get();
	}

	return cloneFuncNode;
}

void SemanticTree::DeleteFuncDefinition(Node* funcNode) const
{
	if (!IsInterpretation) return;

	funcNode->Siblink->Parent = funcNode->Parent;
	funcNode->Parent->Siblink = std::move(funcNode->Siblink);
}

void SemanticTree::AssignParamsWithArgs(const std::vector<std::shared_ptr<DataValue>>& args)
{
	_currNode = _currNode->Child.get();
	size_t argNum = 0;
	while (_currNode->Siblink != nullptr) {
		if (argNum < args.size()) {
			SetVariableValue(_currNode->Siblink.get(), make_shared<DataValue>(*args[argNum]));
			++argNum;
		}
		_currNode = _currNode->Siblink.get();
	}
}

void SemanticTree::AddScope()
{
	if (!IsInterpretation) return;

	_currNode->Child = make_unique<Node>(_currNode);
	SetCurrentNode(_currNode->Child.get());
}

void SemanticTree::Print(std::ostream& out) const
{
	_rootNode->RecursivePrint(out);
}

Node* SemanticTree::FindVariableNodeUp(const std::string& id) const
{
	if (!IsInterpretation) return nullptr;

	auto varNode = FindNodeUp(id);
	if (varNode->GetSemanticType() == SemanticType::Func)
		throw UsingFunctionAsVariableException(id);
	return varNode;
}

Node* SemanticTree::FindFunctionNodeUp(const std::string& id) const
{
	if (!IsInterpretation) return nullptr;

	auto funcNode = FindNodeUp(id);
	if (funcNode->GetSemanticType() != SemanticType::Func)
		throw UsingVariableAsFunctionException(funcNode->Data->Identifier);
	return funcNode;
}

void SemanticTree::DeleteSubTree(Node* node) const
{
	if (!IsInterpretation) return;
	node->Child.reset();
}


// ------------------------ PRIVATE FUNCTIONS ---------------------------


void SemanticTree::SetVariableInitialized(const Node* varNode)
{
	GetVariableData(varNode)->IsInitialized = true;
}

std::vector<DataType> SemanticTree::GetFunctionParams(const Node* funcNode)
{
	const auto funcData = GetFunctionData(funcNode);
	auto paramNode = funcNode->Child->Siblink.get();
	std::vector<DataType> paramsTypes(funcData->ParamsCount);
	for (int i = 0; i < funcData->ParamsCount; i++)
	{
		paramsTypes[i] = paramNode->GetDataType();
		paramNode = paramNode->Siblink.get();
	}
	return paramsTypes;
}

Node* SemanticTree::FindNodeUp(const std::string& id) const
{
	auto node = _currNode;
	while (node->Parent && (node->Data == nullptr || node->Data->Identifier != id)) {
		node = node->Parent;
	}
	if (node->GetSemanticType() == SemanticType::Empty)
		throw UndefinedIdentifierException(id);
	return node;
}

Node* SemanticTree::FindNodeUpInScope(const std::string& id) const
{
	auto node = _currNode;
	auto par = _currNode->Parent;
	while (par && par->Child.get() != node && (node->Data == nullptr || node->Data->Identifier != id)) {
		node = par;
		par = node->Parent;

	}
	return node;
}

bool SemanticTree::GetVariableInitialized(const Node* varNode)
{
	return GetVariableData(varNode)->IsInitialized;
}

VarData* SemanticTree::GetVariableData(const Node* node)
{
	return dynamic_cast<VarData*>(node->Data.get());
}

DataType SemanticTree::GetResultDataType(DataType leftType, DataType rightType, LexemeType operation)
{

	if (leftType == DataType::Long || rightType == DataType::Long)
		return DataType::Long;

	return DataType::Int;
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
		if (lex.str.size() < MAX_INT.size() || lex.str.size() == MAX_INT.size() && lex.str <= MAX_INT)
			return DataType::Int;
		if (lex.str.size() < MAX_LONG.size() || lex.str.size() == MAX_LONG.size() && lex.str <= MAX_LONG)
			return DataType::Long;
		return  DataType::Unknown;
	}
	if (lex.type == LexemeType::OctNum)
	{
		if (lex.str.size() < MAX_INT_O.size() || lex.str.size() == MAX_INT_O.size() && lex.str <= MAX_INT_O)
			return DataType::Int;
		if (lex.str.size() < MAX_LONG_O.size() || lex.str.size() == MAX_LONG_O.size() && lex.str <= MAX_LONG_O)
			return DataType::Long;
		return  DataType::Unknown;
	}
	if (lex.type == LexemeType::HexNum)
	{
		auto suff = lex.str.substr(2);
		if (suff.size() < MAX_INT_H.size() || suff.size() == MAX_INT_H.size() && suff <= MAX_INT_H)
			return DataType::Int;
		if (suff.size() < MAX_LONG_H.size() || suff.size() == MAX_LONG_H.size() && suff <= MAX_LONG_H)
			return DataType::Long;
		return  DataType::Unknown;
	}
	return DataType::Unknown;
}

FuncData* SemanticTree::GetFunctionData(const Node* funcNode)
{
	return dynamic_cast<FuncData*>(funcNode->Data.get());
}


bool SemanticTree::CheckUniqueIdentifier(const std::string& id) const
{
	auto node = FindNodeUpInScope(id);
	return node->GetSemanticType() == SemanticType::Empty;
}

void SemanticTree::CheckCastable(DataType from, DataType to)
{
	if (from == DataType::Void || to == DataType::Void
		|| from == DataType::Unknown || to == DataType::Unknown)
		throw UncastableVariableException(from, to);
}