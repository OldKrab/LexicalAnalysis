#include <stack>
#include "SyntaxAnalyser.h"
#include "Exceptions/AnalysisExceptions.h"


void SyntaxAnalyser::PrintAnalysis()
{
	try
	{
		Program();
		interGen->PrintTriads();
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

	interGen->StartFunctionDeclaration(funcNode->Data->Identifier);

	CompStat();

	interGen->EndFunctionDeclaration();

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
			auto [operand, rightType] = AssignExpr();

			semTree->AssignVariable(varNode, rightType);

			operand = interGen->CastOperandIfNeed(rightType, varNode->GetDataType(), operand);
			interGen->AssignVariable(varNode->Data->Identifier, operand);

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

	auto [operand, condType] = AssignExpr();

	semTree->CheckCastable(condType, DataType::Int);
	operand = interGen->CastOperandIfNeed(condType, DataType::Int, operand);

	lex = scanner->NextScan();								// Scan ;
	CheckExpectedLexeme(lex, LexemeType::Semi);

	AssignExpr();

	lex = scanner->NextScan();								// Scan )
	CheckExpectedLexeme(lex, LexemeType::ClosePar);

	Stat();

	semTree->SetCurrentNode(savedNode);
	semTree->DeleteSubTree(savedNode);
}

std::tuple<Operand, DataType> SyntaxAnalyser::AssignExpr()
{
	auto lex = scanner->LookForward(2);
	if (lex.type == LexemeType::Assign)
	{
		lex = scanner->NextScan();										// Scan Id
		CheckExpectedLexeme(lex, LexemeType::Id);

		const auto node = semTree->FindVariableNodeUp(lex.str);

		lex = scanner->NextScan();										// Scan =

		auto [operand, type] = EqualExpr();

		semTree->AssignVariable(node, type);
		operand = interGen->CastOperandIfNeed(type, node->GetDataType(), operand);
		operand = interGen->AssignVariable(node->Data->Identifier, operand);

		return { operand, node->GetDataType() };
	}
	return EqualExpr();
}

std::tuple<Operand, DataType> SyntaxAnalyser::EqualExpr()
{
	auto [leftOperand, leftType] = CmpExpr();
	auto lex = scanner->LookForward(1);
	while (lex.type == LexemeType::E || lex.type == LexemeType::NE)
	{
		lex = scanner->NextScan();											// Scan ==, !=
		auto [rightOperand, rightType] = CmpExpr();

		auto resType = semTree->CheckOperation(leftType, rightType, lex.type);

		rightOperand = interGen->CastOperandIfNeed(rightType, resType, rightOperand);
		leftOperand = interGen->Operation(lex.str, leftOperand, rightOperand);
		leftOperand = interGen->Operation(lex.str, leftOperand, rightOperand);
		leftType = resType;

		lex = scanner->LookForward(1);
	}
	return { leftOperand, leftType };
}

std::tuple<Operand, DataType> SyntaxAnalyser::CmpExpr()
{
	auto [leftOperand, leftType] = AddExpr();
	auto lex = scanner->LookForward(1);
	while (lex.type == LexemeType::G || lex.type == LexemeType::GE
		|| lex.type == LexemeType::L || lex.type == LexemeType::LE)
	{
		lex = scanner->NextScan();													// Scan >, >=, <, <=
		auto [rightOperand, rightType] = AddExpr();

		auto resType = semTree->CheckOperation(leftType, rightType, lex.type);

		leftOperand = interGen->CastOperandIfNeed(leftType, resType, leftOperand);
		rightOperand = interGen->CastOperandIfNeed(rightType, resType, rightOperand);
		leftOperand = interGen->Operation(lex.str, leftOperand, rightOperand);
		leftType = resType;

		lex = scanner->LookForward(1);
	}
	return { leftOperand, leftType };
}

std::tuple<Operand, DataType> SyntaxAnalyser::AddExpr()
{
	auto [leftOperand, leftType] = MultExpr();
	auto lex = scanner->LookForward(1);
	while (lex.type == LexemeType::Plus
		|| lex.type == LexemeType::Minus)
	{
		scanner->NextScan();													// Scan +, -
		auto [rightOperand, rightType] = MultExpr();

		auto resType = semTree->CheckOperation(leftType, rightType, lex.type);

		leftOperand = interGen->CastOperandIfNeed(leftType, resType, leftOperand);
		rightOperand = interGen->CastOperandIfNeed(rightType, resType, rightOperand);
		leftOperand = interGen->Operation(lex.str, leftOperand, rightOperand);
		leftType = resType;

		lex = scanner->LookForward(1);
	}
	return { leftOperand, leftType };
}

std::tuple<Operand, DataType> SyntaxAnalyser::MultExpr()
{
	auto [leftOperand, leftType] = PrefixExpr();
	auto lex = scanner->LookForward(1);
	while (lex.type == LexemeType::Mul
		|| lex.type == LexemeType::Div
		|| lex.type == LexemeType::Modul)
	{
		scanner->NextScan();												// Scan *, /, %
		auto [rightOperand, rightType] = PrefixExpr();

		auto resType = semTree->CheckOperation(leftType, rightType, lex.type);

		leftOperand = interGen->CastOperandIfNeed(leftType, resType, leftOperand);
		rightOperand = interGen->CastOperandIfNeed(rightType, resType, rightOperand);
		leftOperand = interGen->Operation(lex.str, leftOperand, rightOperand);
		leftType = resType;

		lex = scanner->LookForward(1);
	}
	return { leftOperand, leftType };
}

std::tuple<Operand, DataType> SyntaxAnalyser::PrefixExpr()
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
	auto [operand, type] = PostfixExpr();
	while (!ops.empty())
	{
		lex = ops.top();

		auto resType = semTree->CheckPrefixOperation(lex.type, type);

		operand = interGen->CastOperandIfNeed(type, resType, operand);
		operand = interGen->Operation(lex.str, operand);
		type = resType;

		ops.pop();
	}

	return { operand, type };
}

std::tuple<Operand, DataType> SyntaxAnalyser::PostfixExpr()
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

std::tuple<Operand, DataType> SyntaxAnalyser::FuncCall()
{
	auto lex = scanner->NextScan();							// Scan Id, main

	auto funcNode = semTree->FindFunctionNodeUp(lex.str);

	scanner->NextScan();											// Scan (

	std::vector<Operand> args;
	std::vector<DataType> argsTypes;

	lex = scanner->LookForward(1);
	// work with arguments
	if (lex.type != LexemeType::ClosePar)
	{
		do
		{
			auto [operand, type] = AssignExpr();
			args.push_back(operand);
			argsTypes.push_back(type);

			lex = scanner->NextScan();								// Scan ,
		} while (lex.type == LexemeType::Comma);
		CheckExpectedLexeme(lex, LexemeType::ClosePar);

	}
	else
		scanner->NextScan();

	semTree->CheckValidFuncArgs(funcNode, argsTypes);

	auto paramTypes = semTree->GetFunctionParams(funcNode);
	for (size_t i = 0; i < args.size(); i++)
		args[i] = interGen->CastOperandIfNeed(argsTypes[i], paramTypes[i], args[i]);
	auto operand = interGen->CallFunction(funcNode->Data->Identifier, args);

	return { operand, DataType::Void };
}


std::tuple<Operand, DataType> SyntaxAnalyser::PrimExpr()
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
		return { Operand(node->Data->Identifier), node->GetDataType() };
	}

	if (lex.type == LexemeType::DecimNum || lex.type == LexemeType::HexNum
		|| lex.type == LexemeType::OctNum)
	{
		return{ Operand(lex.str), semTree->GetDataTypeOfNum(lex) };
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
