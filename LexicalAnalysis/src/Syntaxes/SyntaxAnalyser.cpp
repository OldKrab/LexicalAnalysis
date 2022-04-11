#include <stack>
#include "SyntaxAnalyser.h"
#include "Exceptions/AnalysisExceptions.h"


void SyntaxAnalyser::PrintAnalysis()
{
	try
	{
		Program();
		semTree->Print();
	}
	catch (AnalysisException& ex)
	{
		auto pos = scanner->GetCurPos();
		std::cout << "(" << pos.row << ", " << pos.column << "): " << ex.what() << std::endl;

	}
}

void SyntaxAnalyser::Program()
{
	auto firstLex = scanner->LookForward(1);
	auto lex = scanner->LookForward(3);
	while (firstLex.type != LexemeType::End) {
		if (lex.type == LexemeType::OpenPar)
			FuncDecl();
		else
			DataDecl();
		firstLex = scanner->LookForward(1);
		lex = scanner->LookForward(3);
	}
}

void SyntaxAnalyser::FuncDecl()
{
	auto lex = scanner->NextScan();				//Scan Void
	if (lex.type != LexemeType::Void)
		throw InvalidTypeException(lex.str);

	lex = scanner->NextScan();							//Scan Id, Main

	const auto funcNode = semTree->AddFunction(lex.str);

	if (lex.type != LexemeType::Id && lex.type != LexemeType::Main)
		throw InvalidIdentifierException(lex.str);

	lex = scanner->NextScan();							//Scan (

	Params(funcNode);

	lex = scanner->NextScan();							//Scan )
	CheckExpectedLexeme(lex, LexemeType::ClosePar);


	semTree->SetFunctionPos(funcNode, scanner->GetCurPos());

	CompStat();

	semTree->SetCurrentNode(funcNode);
}

void SyntaxAnalyser::DataDecl()
{
	auto lex = scanner->NextScan();										//Scan Type
	if (!IsDataType(lex.type))
		throw InvalidTypeException(lex.str);

	const auto leftType = LexemeStringToDataType(lex.str);
	do
	{
		lex = scanner->NextScan();												//Scan Id
		if (lex.type != LexemeType::Id)
			throw InvalidIdentifierException(lex.str);

		const auto varNode = semTree->AddVariable(leftType, lex.str);

		lex = scanner->NextScan();												//Scan '=', ',', ';'

		if (lex.type == LexemeType::Assign) {
			auto rightType = AssignExpr();
			semTree->AssignVariable(varNode, rightType);

			lex = scanner->NextScan();											//Scan  ',', ';'
		}

	} while (lex.type == LexemeType::Comma);

	CheckExpectedLexeme(lex, LexemeType::Semi);
}

void SyntaxAnalyser::Params(const Node* funcNode) const
{
	while (true) {

		auto lex = scanner->LookForward(1);						// Scan Type
		if (lex.type == LexemeType::Id && scanner->LookForward(2).type == LexemeType::Id)
			throw InvalidTypeException(lex.str);

		if (!IsDataType(lex.type))
			return;

		lex = scanner->NextScan();
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
	if (IsDataType(lex.type) || lex.type == LexemeType::Id && scanner->LookForward(2).type == LexemeType::Id)
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

		CheckExpectedLexeme(lex, LexemeType::Semi);

	}
}

void SyntaxAnalyser::CompStat()
{

	scanner->NextScan();						// Scan {

	const auto node = semTree->AddEmpty();		// Add empty node
	semTree->AddScope();						// Go down

	auto lex = scanner->LookForward(1);
	while (lex.type != LexemeType::CloseBrace)
	{
		Stat();
		lex = scanner->LookForward(1);
	}
	lex = scanner->NextScan();					// Scan }

	semTree->SetCurrentNode(node);				// Restore empty node

	semTree->DeleteSubTree(node);
}

void SyntaxAnalyser::For()
{
	scanner->NextScan();									// Scan for

	auto lex = scanner->NextScan();								// Scan (
	CheckExpectedLexeme(lex, LexemeType::OpenPar);

	auto savedNode = semTree->AddEmpty();
	semTree->AddScope();

	DataDecl();

	auto condType = AssignExpr();

	semTree->CheckCastable(condType, DataType::Int);

	lex = scanner->NextScan();								// Scan ;
	CheckExpectedLexeme(lex, LexemeType::Semi);

	AssignExpr();

	lex = scanner->NextScan();								// Scan )
	CheckExpectedLexeme(lex, LexemeType::ClosePar);

	Stat();

	semTree->SetCurrentNode(savedNode);
	semTree->DeleteSubTree(savedNode);
}

DataType SyntaxAnalyser::AssignExpr()
{
	auto lex = scanner->LookForward(2);
	if (lex.type == LexemeType::Assign)
	{
		lex = scanner->NextScan();										// Scan Id
		CheckExpectedLexeme(lex, LexemeType::Id);

		const auto node = semTree->FindVariableNodeUp(lex.str);

		lex = scanner->NextScan();										// Scan =

		semTree->AssignVariable(node, EqualExpr());

		return node->GetDataType();
	}
	return EqualExpr();
}

DataType SyntaxAnalyser::EqualExpr()
{
	auto leftType = CmpExpr();
	auto lex = scanner->LookForward(1);
	while (lex.type == LexemeType::E || lex.type == LexemeType::NE)
	{
		lex = scanner->NextScan();											// Scan ==, !=
		auto rightType = CmpExpr();

		leftType = semTree->PerformOperation(leftType, rightType, lex.type);
		lex = scanner->LookForward(1);
	}
	return leftType;
}

DataType SyntaxAnalyser::CmpExpr()
{
	auto leftType = AddExpr();
	auto lex = scanner->LookForward(1);
	while (lex.type == LexemeType::G || lex.type == LexemeType::GE
		|| lex.type == LexemeType::L || lex.type == LexemeType::LE)
	{
		lex = scanner->NextScan();													// Scan >, >=, <, <=
		const auto rightType = AddExpr();

		leftType = semTree->PerformOperation(leftType, rightType, lex.type);

		lex = scanner->LookForward(1);
	}
	return leftType;
}

DataType SyntaxAnalyser::AddExpr()
{
	auto leftType = MultExpr();
	auto lex = scanner->LookForward(1);
	while (lex.type == LexemeType::Plus
		|| lex.type == LexemeType::Minus)
	{
		scanner->NextScan();													// Scan +, -
		const auto rightType = MultExpr();

		leftType = semTree->PerformOperation(leftType, rightType, lex.type);

		lex = scanner->LookForward(1);
	}
	return leftType;
}

DataType SyntaxAnalyser::MultExpr()
{
	auto leftType = PrefixExpr();
	auto lex = scanner->LookForward(1);
	while (lex.type == LexemeType::Mul
		|| lex.type == LexemeType::Div
		|| lex.type == LexemeType::Modul)
	{
		scanner->NextScan();												// Scan *, /, %
		const auto rightType = PrefixExpr();

		leftType = semTree->PerformOperation(leftType, rightType, lex.type);

		lex = scanner->LookForward(1);
	}
	return leftType;
}

DataType SyntaxAnalyser::PrefixExpr()
{
	auto lex = scanner->LookForward(1);
	std::stack<Lexeme> ops;
	while (lex.type == LexemeType::Inc || lex.type == LexemeType::Dec
		|| lex.type == LexemeType::Plus || lex.type == LexemeType::Minus)
	{
		lex = scanner->NextScan();										// Scan ++, --, +, -
		ops.push(lex);
		lex = scanner->LookForward(1);
	}
	auto value = PostfixExpr();

	while (!ops.empty())
	{
		lex = ops.top();
		value = semTree->PerformPrefixOperation(lex.type, value);

		ops.pop();
	}
	return value;
}

DataType SyntaxAnalyser::PostfixExpr()
{
	auto lex = scanner->LookForward(1);
	auto lex2 = scanner->LookForward(2);
	if ((lex.type == LexemeType::Id || lex.type == LexemeType::Main)
		&& lex2.type == LexemeType::OpenPar)							// func call
	{
		return FuncCall();
	}

	return PrimExpr();
}

DataType SyntaxAnalyser::FuncCall()
{
	auto lex = scanner->NextScan();							// Scan Id, main

	auto funcNode = semTree->FindFunctionNodeUp(lex.str);

	scanner->NextScan();											// Scan (

	std::vector<DataType> args;
	lex = scanner->LookForward(1);
	// work with arguments
	if (lex.type != LexemeType::ClosePar)
	{
		do
		{
			auto type = AssignExpr();
			args.push_back(type);

			lex = scanner->NextScan();								// Scan ,
		} while (lex.type == LexemeType::Comma);
		CheckExpectedLexeme(lex, LexemeType::ClosePar);

	}
	else
		scanner->NextScan();

	semTree->CheckValidFuncArgs(funcNode, args);
	return DataType::Void;
}


DataType SyntaxAnalyser::PrimExpr()
{
	auto lex = scanner->NextScan();								// Scan DecNum, HexNum, OctNum, Id, Main (

	if (lex.type == LexemeType::OpenPar)								// (expr)
	{
		auto resValue = AssignExpr();
		lex = scanner->NextScan();
		CheckExpectedLexeme(lex, LexemeType::ClosePar);
		return resValue;
	}

	if (lex.type == LexemeType::Id || lex.type == LexemeType::Main)		// identifier
	{
		auto node = semTree->FindVariableNodeUp(lex.str);
		semTree->CheckInitialized(node);
		return node->GetDataType();
	}

	if (lex.type == LexemeType::DecimNum || lex.type == LexemeType::HexNum
		|| lex.type == LexemeType::OctNum)
	{
		return semTree->GetDataTypeOfNum(lex);
	}

	throw ExpectedExpressionException(lex);
}

void SyntaxAnalyser::CheckExpectedLexeme(const Lexeme& givenLexeme, LexemeType expectedType)
{
	if (expectedType != givenLexeme.type)
		throw NotExpectedLexemeException(LexemeTypeToString(expectedType), givenLexeme);
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
