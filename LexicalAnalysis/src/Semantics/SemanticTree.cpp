#include "SemanticTree.h"
#include "Types/DataType.h"
#include "Types/LexemeType.h"
#include "Types/SemanticType.h"
#include "Node/FuncData.h"
#include "Node/VarData.h"
#include <memory>

#include "Exceptions/AnalysisExceptions.h"

using std::make_unique;

SemanticTree::SemanticTree()
	:_rootNode(make_unique<Node>(nullptr)),
	_currNode(_rootNode.get())
{}

Node* SemanticTree::GetCurrentNode() const
{
	return  _currNode;
}

void SemanticTree::SetCurrentNode(Node* node)
{

	_currNode = node;
}

Node* SemanticTree::AddVariable(DataType type, const std::string& id)
{

	if (!CheckUniqueIdentifier(id))
		throw RedefinedIdentifierException(id);

	_currNode->Siblink = make_unique<Node>(_currNode, make_unique<VarData>(id, type));
	SetCurrentNode(_currNode->Siblink.get());
	return _currNode;
}

void SemanticTree::AssignVariable(const Node* node, DataType type)
{
	auto varData = GetVariableData(node);
	CheckCastable(type, varData->Type);
	SetVariableInitialized(node);
}

void SemanticTree::CheckInitialized(const Node* node)
{
	if (!GetVariableInitialized(node))
		throw UsingUninitializedVariableException(node->Data->Identifier);
}

DataType SemanticTree::CheckOperation(DataType leftType, DataType rightType, LexemeType operation)
{
	auto resType = DataType::Int;
	if (leftType == DataType::Long || rightType == DataType::Long)
		resType = DataType::Long;
	CheckCastable(leftType, resType);
	CheckCastable(rightType, resType);
	return resType;
}



DataType SemanticTree::CheckPrefixOperation(LexemeType operation, DataType type) 
{
	auto resType = DataType::Int;
	if (type == DataType::Long)
		resType = DataType::Long;
	CheckCastable(type, resType);
	return resType;
}

void SemanticTree::CheckOperationValid(DataType leftType, DataType rightType, LexemeType operation)
{
	if (leftType == DataType::Void || rightType == DataType::Void
		|| leftType == DataType::Unknown || rightType == DataType::Unknown)
		throw InvalidOperandsException(leftType, rightType, LexemeTypeToString(operation));

	if (operation == LexemeType::Div || operation == LexemeType::Modul)
		throw DivisionOnZeroException();
}

void SemanticTree::CheckOperationValid(DataType type, LexemeType operation)
{
	if (type == DataType::Unknown || type == DataType::Void)
		throw InvalidOperandsException(type, LexemeTypeToString(operation));
}

void SemanticTree::CheckValidFuncArgs(const Node* funcNode, const std::vector<DataType>& args)
{
	auto paramsTypes = GetFunctionParams(funcNode);

	if (args.size() != paramsTypes.size())
		throw WrongArgsCountException(paramsTypes.size(), args.size(), funcNode->Data->Identifier);

	for (size_t i = 0; i < args.size(); i++)
		CheckCastable(args[i], paramsTypes[i]);
}


Node* SemanticTree::AddFunction(const std::string& id)
{
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
	_currNode->Siblink = make_unique<Node>(_currNode);
	SetCurrentNode(_currNode->Siblink.get());
	return _currNode;
}

void SemanticTree::AddParam(const Node* funcNode, const std::string& id, DataType type)
{
	auto funcData = GetFunctionData(funcNode);
	funcData->ParamsCount++;
	auto node = AddVariable(type, id);
	SetVariableInitialized(node);
}


void SemanticTree::SetFunctionPos(const Node* funcNode, const SourceText::Iterator& pos)
{
	GetFunctionData(funcNode)->Pos = pos;
}

SourceText::Iterator SemanticTree::GetFunctionPos(const Node* funcNode)
{
	return GetFunctionData(funcNode)->Pos;
}

Node* SemanticTree::CloneFunctionDefinition(Node* origNode) const
{
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
	funcNode->Siblink->Parent = funcNode->Parent;
	funcNode->Parent->Siblink = std::move(funcNode->Siblink);
}

void SemanticTree::AddScope()
{
	_currNode->Child = make_unique<Node>(_currNode);
	SetCurrentNode(_currNode->Child.get());
}

void SemanticTree::Print(std::ostream& out) const
{
	_rootNode->RecursivePrint(out);
}

Node* SemanticTree::FindVariableNodeUp(const std::string& id) const
{
	auto varNode = FindNodeUp(id);
	if (varNode->GetSemanticType() == SemanticType::Func)
		throw UsingFunctionAsVariableException(id);
	return varNode;
}

Node* SemanticTree::FindFunctionNodeUp(const std::string& id) const
{
	auto funcNode = FindNodeUp(id);
	if (funcNode->GetSemanticType() != SemanticType::Func)
		throw UsingVariableAsFunctionException(funcNode->Data->Identifier);
	return funcNode;
}

void SemanticTree::DeleteSubTree(Node* node)
{
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
		throw InvalidNumberException();
	}
	if (lex.type == LexemeType::OctNum)
	{
		if (lex.str.size() < MAX_INT_O.size() || lex.str.size() == MAX_INT_O.size() && lex.str <= MAX_INT_O)
			return DataType::Int;
		if (lex.str.size() < MAX_LONG_O.size() || lex.str.size() == MAX_LONG_O.size() && lex.str <= MAX_LONG_O)
			return DataType::Long;
		throw InvalidNumberException();
	}
	if (lex.type == LexemeType::HexNum)
	{
		auto suff = lex.str.substr(2);
		if (suff.size() < MAX_INT_H.size() || suff.size() == MAX_INT_H.size() && suff <= MAX_INT_H)
			return DataType::Int;
		if (suff.size() < MAX_LONG_H.size() || suff.size() == MAX_LONG_H.size() && suff <= MAX_LONG_H)
			return DataType::Long;
		throw InvalidNumberException();
	}
	throw InvalidNumberException();
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