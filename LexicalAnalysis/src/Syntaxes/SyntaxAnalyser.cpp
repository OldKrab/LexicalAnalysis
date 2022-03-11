#include <stack>
#include "SyntaxAnalyser.h"
#include "Exceptions/AnalysisExceptions.h"


void SyntaxAnalyser::PrintAnalysis()
{
	/*try
	{
		Program();
	}
	catch (AnalysisException& ex)
	{
		auto pos = scanner->GetCurPos();
		std::cout << "(" << pos.row << ", " << pos.column << "): " << ex.what() << std::endl;
	}*/
		Program();

	semTree->Print();
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
	auto lex = scanner->NextScan();				//Scan Void
	CheckExpectedLexeme(lex, LexemeType::Void);

	lex = scanner->NextScan();							//Scan Id, Main

	const auto funcNode = semTree->AddFunction(lex.str);

	if (lex.type != LexemeType::Id && lex.type != LexemeType::Main)
		throw InvalidIdentifierException(lex.str);

	const auto isMain = lex.type == LexemeType::Main;

	lex = scanner->NextScan();							//Scan (
	CheckExpectedLexeme(lex, LexemeType::OpenPar);

	Params(funcNode);

	lex = scanner->NextScan();							//Scan )
	CheckExpectedLexeme(lex, LexemeType::ClosePar);


	semTree->SetFunctionPos(funcNode, scanner->GetCurPos());
	if (!isMain)
		semTree->IsInterpretation = false;

	CompStat();

	semTree->IsInterpretation = true;

	semTree->SetCurrentNode(funcNode);
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

			semTree->SetVariableValue(varNode, rightValue);

			lex = scanner->NextScan();											//Scan  ',', ';'
		}

	} while (lex.type == LexemeType::Comma);

	CheckExpectedLexeme(lex, LexemeType::Semi);
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

		CheckExpectedLexeme(lex, LexemeType::Semi);

	}
}

void SyntaxAnalyser::CompStat()
{

	auto lex = scanner->NextScan();		// Scan {

	CheckExpectedLexeme(lex, LexemeType::OpenBrace);

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

	semTree->DeleteSubTree(node);
}

void SyntaxAnalyser::For()
{
	auto savedIsInterpret = semTree->IsInterpretation;

	auto lex = scanner->NextScan();					// Scan for
	CheckExpectedLexeme(lex, LexemeType::For);

	lex = scanner->NextScan();								// Scan (
	CheckExpectedLexeme(lex, LexemeType::OpenPar);

	auto savedNode = semTree->AddEmpty();
	semTree->AddScope();

	DataDecl();

	auto condPos = scanner->GetCurPos();
	auto condValue = AssignExpr();
	condValue = semTree->CastValue(condValue, DataType::Int);


	lex = scanner->NextScan();								// Scan ;
	CheckExpectedLexeme(lex, LexemeType::Semi);

	const auto exprPos = scanner->GetCurPos();
	semTree->IsInterpretation = false;
	AssignExpr();
	semTree->IsInterpretation = savedIsInterpret && condValue.intVal != 0;

	lex = scanner->NextScan();								// Scan )
	CheckExpectedLexeme(lex, LexemeType::ClosePar);

	SourceText::Iterator statStartPos = scanner->GetCurPos(), statEndPos;

	do
	{
		scanner->SetCurPos(statStartPos);
		Stat();
		statEndPos = scanner->GetCurPos();

		scanner->SetCurPos(exprPos);
		AssignExpr();

		scanner->SetCurPos(condPos);
		condValue = semTree->CastValue(AssignExpr(), DataType::Int);
		semTree->IsInterpretation = savedIsInterpret && condValue.intVal != 0;

	} while (semTree->IsInterpretation);

	scanner->SetCurPos(statEndPos);
	semTree->IsInterpretation = savedIsInterpret;

	semTree->SetCurrentNode(savedNode);
	semTree->DeleteSubTree(savedNode);
}

DataValue SyntaxAnalyser::AssignExpr()
{
	auto lex = scanner->LookForward(2);
	if (lex.type == LexemeType::Assign)
	{
		lex = scanner->NextScan();										// Scan Id
		CheckExpectedLexeme(lex, LexemeType::Id);

		const auto node = semTree->FindVariableNodeUp(lex.str);

		lex = scanner->NextScan();										// Scan =

		semTree->SetVariableValue(node, EqualExpr());

		return semTree->GetVariableValue(node);
	}
	return EqualExpr();
}

DataValue SyntaxAnalyser::EqualExpr()
{
	auto leftValue = CmpExpr();
	auto lex = scanner->LookForward(1);
	while (lex.type == LexemeType::E || lex.type == LexemeType::NE)
	{
		lex = scanner->NextScan();											// Scan ==, !=
		auto rightValue = CmpExpr();
		semTree->CheckOperationValid(leftValue, rightValue, lex);

		leftValue = semTree->PerformOperation(leftValue, rightValue, lex.type);
		lex = scanner->LookForward(1);
	}
	return leftValue;
}

DataValue SyntaxAnalyser::CmpExpr()
{
	auto leftValue = AddExpr();
	auto lex = scanner->LookForward(1);
	while (lex.type == LexemeType::G || lex.type == LexemeType::GE
		|| lex.type == LexemeType::L || lex.type == LexemeType::LE)
	{
		lex = scanner->NextScan();													// Scan >, >=, <, <=
		const auto rightValue = AddExpr();

		semTree->CheckOperationValid(leftValue, rightValue, lex);

		leftValue = semTree->PerformOperation(leftValue, rightValue, lex.type);

		lex = scanner->LookForward(1);
	}
	return leftValue;
}

DataValue SyntaxAnalyser::AddExpr()
{
	auto leftValue = MultExpr();
	auto lex = scanner->LookForward(1);
	while (lex.type == LexemeType::Plus
		|| lex.type == LexemeType::Minus)
	{
		scanner->NextScan();													// Scan +, -
		const auto rightValue = MultExpr();

		semTree->CheckOperationValid(leftValue, rightValue, lex);

		leftValue = semTree->PerformOperation(leftValue, rightValue, lex.type);

		lex = scanner->LookForward(1);
	}
	return leftValue;
}

DataValue SyntaxAnalyser::MultExpr()
{
	auto leftValue = PrefixExpr();
	auto lex = scanner->LookForward(1);
	while (lex.type == LexemeType::Mul
		|| lex.type == LexemeType::Div
		|| lex.type == LexemeType::Modul)
	{
		scanner->NextScan();												// Scan *, /, %
		const auto rightValue = PrefixExpr();

		semTree->CheckOperationValid(leftValue, rightValue, lex);

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
		lex = scanner->NextScan();										// Scan ++, --, +, -
		ops.push(lex);
		lex = scanner->LookForward(1);
	}
	auto value = PostfixExpr();

	while (!ops.empty())
	{
		lex = ops.top();
		semTree->CheckOperationValid(value.type, lex);
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

	auto funcNode = semTree->FindFunctionNodeUp(lex.str);

	scanner->NextScan();											// Scan (

	std::vector<DataValue> args;
	lex = scanner->LookForward(1);
	// work with arguments
	if (lex.type != LexemeType::ClosePar)
	{
		do
		{
			args.push_back(AssignExpr());

			lex = scanner->NextScan();								// Scan ,
		} while (lex.type == LexemeType::Comma);
		CheckExpectedLexeme(lex, LexemeType::ClosePar);

	}
	else
		scanner->NextScan();

	semTree->CheckValidFuncArgs(funcNode, args);

	if (semTree->IsInterpretation)
	{
		auto savedPos = scanner->GetCurPos();
		auto savedNode = semTree->GetCurrentNode();

		scanner->SetCurPos(semTree->GetFunctionPos(funcNode));
		auto calledFunc = semTree->CloneFunctionDefinition(funcNode);
		CompStat();
		semTree->DeleteFuncDefinition(calledFunc);

		semTree->SetCurrentNode(savedNode);
		scanner->SetCurPos(savedPos);
	}
}


DataValue SyntaxAnalyser::PrimExpr()
{
	auto lex = scanner->NextScan();								// Scan DecNum, HexNum, OctNum, Id, Main (

	if (lex.type == LexemeType::OpenPar)								// (expr)
	{
		const auto resValue = AssignExpr();
		lex = scanner->NextScan();
		CheckExpectedLexeme(lex, LexemeType::ClosePar);
		return resValue;
	}

	if (lex.type == LexemeType::Id || lex.type == LexemeType::Main)		// identifier
	{
		return semTree->GetVariableValue(semTree->FindVariableNodeUp(lex.str));
	}

	if (lex.type == LexemeType::DecimNum || lex.type == LexemeType::HexNum
		|| lex.type == LexemeType::OctNum)
	{
		return semTree->GetValueOfNum(lex);
	}

	throw UnknownLexemeException(lex);
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
