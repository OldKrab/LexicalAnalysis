#include "SyntaxAnalyser.h"

#include <stack>
#include "Exceptions/AnalysisExceptions.h"
#include "Semantics/Node/VarData.h"
#include "Types/SemanticType.h"


void SyntaxAnalyser::StartAnalysis()
{
	try
	{
		Program();
	}
	catch (AnalysisException& ex)
	{
		auto pos = scanner->GetCurPos();
		std::cout << "(" << pos.row << ", " << pos.column << "): " << ex.what() << std::endl;
	}
}

void SyntaxAnalyser::Program()
{
	auto lex = scanner->LookForward(1);
	while (lex.type != LexemeType::End) {
		if (IsDataType(lex.type))
			DataDecl();
		else if (lex.type == LexemeType::Void)
			FuncDecl();
		else
			throw InvalidTypeException(lex.str);
		lex = scanner->LookForward(1);
	}
}

void SyntaxAnalyser::FuncDecl()
{
	auto lex = scanner->NextScan();			//Scan Void
	if (lex.type != LexemeType::Void)
		throw NotExpectedLexemeException("void", lex);

	lex = scanner->NextScan();						//Scan Id, Main

	if (!semTree->CheckUniqueIdentifier(lex.str))					// Check unique id
		throw RedefinedIdentifierException(lex.str);


	const auto funcNode = semTree->AddFunc(lex.str);		// Create func node

	if (lex.type == LexemeType::Id)
	{
		lex = scanner->NextScan();					//Scan (
		if (lex.type != LexemeType::OpenPar)
			throw NotExpectedLexemeException("(", lex);
		Params(funcNode);
	}
	else if (lex.type == LexemeType::Main)
	{
		lex = scanner->NextScan();					//Scan (
		if (lex.type != LexemeType::OpenPar)
			throw NotExpectedLexemeException("(", lex);
	}
	else
		throw InvalidIdentifierException(lex.str);

	lex = scanner->NextScan();						//Scan )
	if (lex.type != LexemeType::ClosePar)
		throw NotExpectedLexemeException(")", lex);

	CompStat();

	semTree->SetCurrentNode(funcNode);				// Restore func node
}

void SyntaxAnalyser::DataDecl()
{
	auto lex = scanner->NextScan();								//Scan Type
	if (!IsDataType(lex.type))
		throw InvalidTypeException(lex.str);

	const auto leftType = LexemeStringToDataType(lex.str);				// Get data type
	do
	{
		lex = scanner->NextScan();										//Scan Id
		if (lex.type != LexemeType::Id)
			throw InvalidIdentifierException(lex.str);

		const auto varNode = semTree->AddVariable(leftType, lex.str);			// Add variable node

		lex = scanner->NextScan();												//Scan '=', ',', ';'

		if (lex.type == LexemeType::Assign) {
			const auto rightValue = AssignExpr();								// Get data type of right expression

			if (!semTree->CheckCastable(rightValue.type, leftType))		// Check castable
				throw UncastableVariableException(rightValue.type, leftType);

			semTree->SetVariableValue(varNode, rightValue);

			lex = scanner->NextScan();											//Scan  ',', ';'
		}

	} while (lex.type == LexemeType::Comma);

	if (lex.type != LexemeType::Semi)
		throw NotExpectedLexemeException(";", lex);
}



void SyntaxAnalyser::Params(Node* funcNode) const
{
	auto lex = scanner->LookForward(1);
	if (!IsDataType(lex.type))
		return;

	while (true) {
		lex = scanner->NextScan();						// Scan Type
		if (!IsDataType(lex.type))
			throw InvalidTypeException(lex.str);

		const auto type = LexemeStringToDataType(lex.str);	// Get data type

		lex = scanner->NextScan();						// Scan Id
		if (lex.type != LexemeType::Id)
			throw InvalidIdentifierException(lex.str);

		semTree->AddParam(funcNode, lex.str, type);		// Add var node to func as param

		lex = scanner->LookForward(1);
		if (lex.type != LexemeType::Comma)
			return;

		lex = scanner->NextScan();						// Scan ,
	}
}

void SyntaxAnalyser::Stat()
{
	auto lex = scanner->LookForward(1);
	if (IsDataType(lex.type))
		DataDecl();
	else if (lex.type == LexemeType::OpenBrace)
		CompStat();
	else if (lex.type == LexemeType::For)
		For();
	else {
		// Expressions
		if (lex.type != LexemeType::Semi)
			AssignExpr();

		lex = scanner->NextScan();					// Scan ;
		if (lex.type != LexemeType::Semi)
			throw NotExpectedLexemeException(";", lex);
	}
}

void SyntaxAnalyser::CompStat()
{

	auto lex = scanner->NextScan();		// Scan {
	if (lex.type != LexemeType::OpenBrace)
		throw NotExpectedLexemeException("{", lex);

	const auto node = semTree->AddEmpty();		// Add empty node
	semTree->AddScope();						// Go down

	lex = scanner->LookForward(1);
	while (lex.type != LexemeType::CloseBrace)
	{
		Stat();
		lex = scanner->LookForward(1);
	}
	lex = scanner->NextScan();					// Scan }

	semTree->SetCurrentNode(node);				// Restore empty node



	node->RightChild.reset();					// Delete subtree
}

void SyntaxAnalyser::For()
{
	auto lex = scanner->NextScan();		// Scan for
	if (lex.type != LexemeType::For)
		throw NotExpectedLexemeException("for", lex);

	lex = scanner->NextScan();					// Scan (
	if (lex.type != LexemeType::OpenPar)
		throw NotExpectedLexemeException("(", lex);

	auto node = semTree->AddEmpty();		// Add empty node, save it
	semTree->AddScope();
	DataDecl();

	lex = scanner->LookForward(1);
	const auto condValue = AssignExpr();
	if (!semTree->CheckCastable(condValue.type, DataType::Int))
		throw UncastableVariableException(condValue.type, DataType::Int);

	lex = scanner->NextScan();					// Scan ;
	if (lex.type != LexemeType::Semi)
		throw NotExpectedLexemeException(";", lex);

	AssignExpr();

	lex = scanner->NextScan();					// Scan )
	if (lex.type != LexemeType::ClosePar)
		throw NotExpectedLexemeException(")", lex);

	Stat();

	semTree->SetCurrentNode(node);


	node->RightChild.reset();	// Delete subtree
}

DataValue SyntaxAnalyser::AssignExpr()
{
	auto lex = scanner->LookForward(2);
	if (lex.type == LexemeType::Assign)
	{
		lex = scanner->NextScan();												// Scan Id
		if (lex.type != LexemeType::Id)
			throw InvalidIdentifierException(lex.str);

		const auto node = semTree->FindNodeUp(lex.str);
		if (node->GetSemanticType() == SemanticType::Empty)						// Check undefined var
			throw UndefinedIdentifierException(lex.str);
		if (node->GetSemanticType() == SemanticType::Func)						// Check assign to func
			throw UsingFunctionAsVariableException(lex.str);

		lex = scanner->NextScan();												// Scan =

		const auto leftValue = semTree->GetVariableValue(node);					// Get value of expressions
		auto rightValue = EqualExpr();

		if (!semTree->CheckCastable(rightValue.type, leftValue.type))	// Check castable right to left
			throw UncastableVariableException(leftValue.type, rightValue.type);

		semTree->SetVariableValue(node, rightValue);							// Set right value

		return semTree->GetVariableValue(node);
	}
	return  EqualExpr();
}

DataValue SyntaxAnalyser::EqualExpr()
{
	auto leftValue = CmpExpr();												// Get value of left expr
	auto lex = scanner->LookForward(1);
	while (lex.type == LexemeType::E || lex.type == LexemeType::NE)
	{
		lex = scanner->NextScan();											// Scan ==, !=
		auto rightValue = CmpExpr();										// Get value of right expr

		CheckOperationValid(leftValue, rightValue, lex);

		leftValue = semTree->PerformOperation(leftValue, rightValue, lex.type);
		lex = scanner->LookForward(1);
	}
	return leftValue;
}

DataValue SyntaxAnalyser::CmpExpr()
{
	auto leftValue = AddExpr();														// Get value of left expr
	auto lex = scanner->LookForward(1);
	while (lex.type == LexemeType::G || lex.type == LexemeType::GE
		|| lex.type == LexemeType::L || lex.type == LexemeType::LE)
	{
		lex = scanner->NextScan();													// Scan >, >=, <, <=
		const auto rightValue = AddExpr();											// Get sem type of right expr

		CheckOperationValid(leftValue, rightValue, lex);	// Check operation is valid

		leftValue = semTree->PerformOperation(leftValue, rightValue, lex.type);

		lex = scanner->LookForward(1);
	}
	return leftValue;
}

DataValue SyntaxAnalyser::AddExpr()
{
	auto leftValue = MultExpr();												// Get sem type of left expr
	auto lex = scanner->LookForward(1);
	while (lex.type == LexemeType::Plus
		|| lex.type == LexemeType::Minus)
	{
		scanner->NextScan();													// Scan +, -
		const auto rightValue = MultExpr();										// Get sem type of right expr

		CheckOperationValid(leftValue, rightValue, lex);	// Check operation is valid and get sem type

		leftValue = semTree->PerformOperation(leftValue, rightValue, lex.type);

		lex = scanner->LookForward(1);
	}
	return leftValue;
}

DataValue SyntaxAnalyser::MultExpr()
{
	auto leftValue = PrefixExpr();									// Get sem type of left expr
	auto lex = scanner->LookForward(1);
	while (lex.type == LexemeType::Mul
		|| lex.type == LexemeType::Div
		|| lex.type == LexemeType::Modul)
	{
		scanner->NextScan();										// Scan *, /, %
		const auto rightValue = PrefixExpr();								// Get sem type of right expr

		CheckOperationValid(leftValue, rightValue, lex);	// Check operation is valid and get sem type

		leftValue = semTree->PerformOperation(leftValue, rightValue, lex.type);

		lex = scanner->LookForward(1);
	}
	return leftValue;
}

DataValue SyntaxAnalyser::PrefixExpr()
{
	auto lex = scanner->LookForward(1);
	std::stack<Lexeme> ops;
	while (lex.type == LexemeType::Inc || lex.type == LexemeType::Dec
		|| lex.type == LexemeType::Plus || lex.type == LexemeType::Minus)
	{
		lex = scanner->NextScan();								// Scan ++, --, +, -
		ops.push(lex);
		lex = scanner->LookForward(1);
	}
	auto value = PostfixExpr();

	while (!ops.empty())
	{
		lex = ops.top();
		CheckOperationValid(value.type, lex);	// Check operation is valid and get sem type
		value = semTree->PerformPrefixOperation(lex.type, value);

		ops.pop();
	}
	return value;
}

DataValue SyntaxAnalyser::PostfixExpr()
{
	auto lex = scanner->LookForward(1);
	auto lex2 = scanner->LookForward(2);
	if ((lex.type == LexemeType::Id || lex.type == LexemeType::Main)
		&& lex2.type == LexemeType::OpenPar)							// func call
	{
		FuncCall();
		return DataValue(DataType::Void);
	}

	return PrimExpr();
}

void SyntaxAnalyser::FuncCall()
{
	auto lex = scanner->NextScan();							// Scan Id, main

	auto funcNode = semTree->FindNodeUp(lex.str);
	if (funcNode->GetSemanticType() == SemanticType::Empty)			// Check defined
		throw UndefinedIdentifierException(lex.str);
	if (funcNode->GetSemanticType() != SemanticType::Func)			// Check is func
		throw UsingVariableAsFunctionException(funcNode->Data->Identifier);

	scanner->NextScan();											// Scan (

	auto paramsTypes = semTree->GetFuncParams(funcNode);	// Get func params types
	size_t argsCount = 0;

	lex = scanner->LookForward(1);
	// work with arguments
	if (lex.type != LexemeType::ClosePar)
	{
		while (true)
		{
			lex = scanner->LookForward(1);						// Save pos of argument
			auto value = AssignExpr();								// Get argument value

			if (argsCount < paramsTypes.size() && !semTree->CheckCastable(value.type, paramsTypes[argsCount]))
				throw UncastableArgumentException(paramsTypes[argsCount], value.type, argsCount + 1, lex.str);

			++argsCount;

			lex = scanner->LookForward(1);
			if (lex.type != LexemeType::Comma)
				break;

			lex = scanner->NextScan();								// Scan ,
		}
	}

	if (argsCount != paramsTypes.size())
		throw WrongArgsCountException(paramsTypes.size(), argsCount, funcNode->Data->Identifier);

	scanner->NextScan();											// Scan )
	if (lex.type != LexemeType::ClosePar)
		throw NotExpectedLexemeException(")", lex);
}


DataValue SyntaxAnalyser::PrimExpr()
{
	auto lex = scanner->NextScan();								// Scan DecNum, HexNum, OctNum, Id, Main (

	if (lex.type == LexemeType::OpenPar)								// (expr)
	{
		const auto resValue = AssignExpr();
		lex = scanner->NextScan();
		if (lex.type != LexemeType::ClosePar)
			throw NotExpectedLexemeException(")", lex);
		return resValue;
	}

	if (lex.type == LexemeType::Id || lex.type == LexemeType::Main)		// identifier
	{
		const auto node = semTree->FindNodeUp(lex.str);
		if (node->GetSemanticType() == SemanticType::Empty)
			throw UndefinedIdentifierException(lex.str);
		if (node->GetSemanticType() == SemanticType::Func)
			throw UsingFunctionAsVariableException(lex.str);
		if (!semTree->GetVariableInitialized(node))
			throw UsingUninitializedVariableException(node->Data->Identifier);

		return semTree->GetVariableValue(node);
	}

	if (lex.type == LexemeType::DecimNum || lex.type == LexemeType::HexNum
		|| lex.type == LexemeType::OctNum)
	{
		const auto numValue = semTree->GetValueOfNum(lex);
		if (numValue.type == DataType::Unknown)
			throw InvalidNumberException();
		return numValue;
	}

	throw UnknownLexemeException(lex);
}

DataType SyntaxAnalyser::CheckOperationValid(DataValue leftValue, DataValue rightValue, const Lexeme& lex) const
{
	const auto resType = semTree->GetResultDataType(leftValue.type, rightValue.type, lex.type);
	if (resType == DataType::Unknown)
		throw InvalidOperandsException(leftValue.type, rightValue.type, lex.str);
	if ((lex.type == LexemeType::Div || lex.type == LexemeType::Modul)
		&& (rightValue.type == DataType::Long && rightValue.longVal == 0
			|| rightValue.type == DataType::Int && rightValue.intVal == 0))
		throw DivisionOnZeroException();
	return resType;
}


DataType SyntaxAnalyser::CheckOperationValid(DataType type, const Lexeme& lex) const
{
	if (type == DataType::Unknown)
		throw InvalidOperandsException(type, lex.str);
	return type;
}

bool SyntaxAnalyser::IsTypeForward(LexemeType type, int distance) const
{
	auto lex = scanner->LookForward(distance);
	return lex.type == type;
}


bool SyntaxAnalyser::IsDataType(LexemeType code)
{
	return code == LexemeType::Int || code == LexemeType::Long;
}
