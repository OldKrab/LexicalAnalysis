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

	_currNode->Siblink = std::make_unique<Node>(_currNode);
	_currNode->Siblink->Data = std::make_unique<VarData>(id, type);
	SetCurrentNode(_currNode->Siblink.get());
	return _currNode;
}

DataValue SemanticTree::GetVariableValue(const Node* node) const
{
	if (!IsInterpretation) return {};

	if (!GetVariableInitialized(node))
		throw UsingUninitializedVariableException(node->Data->Identifier);

	return GetVariableData(node)->Value;
}

DataValue SemanticTree::CastValue(DataValue value, DataType type) const
{
	if (!IsInterpretation) return {};

	CheckCastable(value.type, DataType::Int);
	if (value.type == type)
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

	return value;
}

DataValue SemanticTree::PerformOperation(DataValue leftValue, DataValue rightValue, LexemeType operation) const
{
	if (!IsInterpretation) return {};

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

	return resValue;
}

DataValue SemanticTree::PerformPrefixOperation(LexemeType operation, DataValue value) const
{
	if (!IsInterpretation) return {};

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

	return resValue;
}

DataValue SemanticTree::GetValueOfNum(Lexeme lex)
{
	if (!IsInterpretation) return {};

	auto type = GetDataTypeOfNum(lex);
	switch (type)
	{
	case DataType::Int:
		return DataValue(std::stoi(lex.str, nullptr, 0));
	case DataType::Long:
		return DataValue(std::stoll(lex.str, nullptr, 0));
	default:
		throw InvalidNumberException();
	}
}

void SemanticTree::SetVariableValue(Node* node, DataValue value) const
{
	if (!IsInterpretation) return;

	auto varData = GetVariableData(node);
	CheckCastable(value.type, varData->Type);

	value = CastValue(value, node->GetDataType());
	varData->Value = value;
	SetVariableInitialized(node);
}

void SemanticTree::CheckOperationValid(DataValue leftValue, DataValue rightValue, const Lexeme& lex) const
{
	if (!IsInterpretation) return;

	const auto resType = GetResultDataType(leftValue.type, rightValue.type, lex.type);
	if (resType == DataType::Unknown)
		throw InvalidOperandsException(leftValue.type, rightValue.type, lex.str);
	if ((lex.type == LexemeType::Div || lex.type == LexemeType::Modul)
		&& (rightValue.type == DataType::Long && rightValue.longVal == 0
			|| rightValue.type == DataType::Int && rightValue.intVal == 0))
		throw DivisionOnZeroException();
}

void SemanticTree::CheckOperationValid(DataType type, const Lexeme& lex) const
{
	if (!IsInterpretation) return;

	if (type == DataType::Unknown)
		throw InvalidOperandsException(type, lex.str);
}

void SemanticTree::CheckValidFuncArgs(const Node* funcNode, std::vector<DataValue> args) const
{
	if (!IsInterpretation) return;

	auto paramsTypes = GetFunctionParams(funcNode);
	if (args.size() != paramsTypes.size())
		throw WrongArgsCountException(paramsTypes.size(), args.size(), funcNode->Data->Identifier);
	for (size_t i = 0; i < args.size(); i++)
		CheckCastable(args[i].type, paramsTypes[i]);
}

void SemanticTree::CastOperands(DataValue& leftValue, DataValue& rightValue, LexemeType operation) const
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

Node* SemanticTree::AddFunction(const std::string& id)
{
	if (!IsInterpretation) return nullptr;

	if (!CheckUniqueIdentifier(id))			// Check unique id
		throw RedefinedIdentifierException(id);

	_currNode->Siblink = std::make_unique<Node>(_currNode);
	_currNode->Siblink->Data = std::make_unique<FuncData>(id);
	const auto funcNode = _currNode->Siblink.get();
	SetCurrentNode(funcNode);
	AddScope();
	return funcNode;
}

Node* SemanticTree::AddEmpty()
{
	if (!IsInterpretation) return nullptr;

	_currNode->Siblink = std::make_unique<Node>(_currNode);
	SetCurrentNode(_currNode->Siblink.get());
	return _currNode;
}

void SemanticTree::AddParam(Node* funcNode, const std::string& id, DataType type)
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


Node* SemanticTree::CloneFunctionDefinition(Node* node) const
{
	if (!IsInterpretation) return nullptr;

	auto siblink = std::move(node->Siblink);
	node->Siblink = node->Clone();
	node->Siblink->Siblink = std::move(siblink);
	node->Siblink->Parent = node;

	auto origNode = node;
	auto cloneNode = node->Siblink.get();

	origNode = origNode->Child.get();
	cloneNode->Child = origNode->Clone();
	cloneNode = cloneNode->Child.get();
	while (origNode->Siblink != nullptr)
	{
		cloneNode->Siblink = origNode->Siblink->Clone();
		origNode = origNode->Siblink.get();
		cloneNode = cloneNode->Siblink.get();
	}

	return node->Siblink.get();
}

Node* SemanticTree::GetFunctionBodyNode(Node* funcNode) const
{
	if (!IsInterpretation) return nullptr;

	funcNode = funcNode->Child.get();
	while (funcNode->Siblink != nullptr)
		funcNode = funcNode->Siblink.get();
	return funcNode;
}

void SemanticTree::DeleteFuncDefinition(Node* funcNode) const
{
	if (!IsInterpretation) return;

	funcNode->Parent->Siblink = std::move(funcNode->Siblink);
}

void SemanticTree::AddScope()
{
	if (!IsInterpretation) return;

	_currNode->Child = std::make_unique<Node>(_currNode);
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

void SemanticTree::DeleteSubTree(Node* node)
{
	if (!IsInterpretation) return;
	node->Child.reset();
}


// ------------------------ PRIVATE FUNCTIONS ---------------------------


void SemanticTree::SetVariableInitialized(const Node* varNode)
{
	GetVariableData(varNode)->IsInitialized = true;
}

std::vector<DataType> SemanticTree::GetFunctionParams(const Node* funcNode) const
{
	if (funcNode->GetSemanticType() != SemanticType::Func)
		throw UsingVariableAsFunctionException(funcNode->Data->Identifier);

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
	if (node->GetSemanticType() == SemanticType::Func)
		throw UsingFunctionAsVariableException(node->Data->Identifier);
	return dynamic_cast<VarData*>(node->Data.get());
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

FuncData* SemanticTree::GetFunctionData(const Node* funcNode)
{
	if (funcNode->GetSemanticType() == SemanticType::Var)
		throw UsingVariableAsFunctionException(funcNode->Data->Identifier);
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