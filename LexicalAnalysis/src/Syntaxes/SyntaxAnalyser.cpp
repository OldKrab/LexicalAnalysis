#include "SyntaxAnalyser.h"

#include <iostream>
#include <stack>
#include "SyntaxAnalysisException.h"
#include "Types/SemanticType.h"


void SyntaxAnalyser::Program()
{
	auto lex = scanner->LookForward(1);
	while (lex.type != LexemeType::End) {
		if (IsType(lex.type))
			DataDecl();
		else if (lex.type == LexemeType::Void)
			FuncDecl();
		else
			WrongType(lex);
		lex = scanner->LookForward(1);
	}
}

void SyntaxAnalyser::FuncDecl()
{
	auto lex = scanner->NextScan();			//Scan Void
	if (lex.type != LexemeType::Void)
		WrongExpected("void", lex);

	lex = scanner->NextScan();						//Scan Id, Main

	if (!semTree->CheckUniqueIdentifier(lex.str))	// Check unique id
		RedefinitionError(lex);

	const auto funcNode = semTree->AddFunc(lex.str);		// Create func node

	if (lex.type == LexemeType::Id)
	{
		lex = scanner->NextScan();					//Scan (
		if (lex.type != LexemeType::OpenPar)
			WrongExpected("(", lex);
		Params(funcNode);
	}
	else if (lex.type == LexemeType::Main)
	{
		lex = scanner->NextScan();					//Scan (
		if (lex.type != LexemeType::OpenPar)
			WrongExpected("(", lex);
	}
	else
		WrongId(lex);

	lex = scanner->NextScan();						//Scan )
	if (lex.type != LexemeType::ClosePar)
		WrongExpected(")", lex);

	CompStat();

	semTree->SetCurrentNode(funcNode);				// Restore func node
}

void SyntaxAnalyser::DataDecl()
{
	auto lex = scanner->NextScan();								//Scan Type
	if (!IsType(lex.type))
		WrongType(lex);

	const auto leftType = LexemeStringToDataType(lex.str);					// Get data type
	do
	{
		lex = scanner->NextScan();										//Scan Id
		if (lex.type != LexemeType::Id)
			WrongId(lex);

		if (!semTree->CheckUniqueIdentifier(lex.str))					// Check unique id
			RedefinitionError(lex);

		const auto varNode = semTree->AddVariable(leftType, lex.str);			// Add variable node

		lex = scanner->NextScan();										//Scan '=', ',', ';'

		if (lex.type == LexemeType::Assign) {
			const auto rightType = AssignExpr();								// Get data type of right expression

			if (!semTree->CheckCastable(rightType, leftType))	// Check castable
				UncastableError(leftType, rightType, lex);

			semTree->SetVariableInitialized(varNode);					// Set var is initialized

			lex = scanner->NextScan();									//Scan  ',', ';'
		}

	} while (lex.type == LexemeType::Comma);

	if (lex.type != LexemeType::Semi)
		WrongExpected(";", lex);
}



void SyntaxAnalyser::Params(Node* funcNode) const
{
	auto lex = scanner->LookForward(1);
	if (!IsType(lex.type))
		return;

	while (true) {
		lex = scanner->NextScan();						// Scan Type
		if (!IsType(lex.type))
			WrongType(lex);

		const auto type = LexemeStringToDataType(lex.str);	// Get data type

		lex = scanner->NextScan();						// Scan Id
		if (lex.type != LexemeType::Id)
			WrongId(lex);

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
	if (IsType(lex.type))
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
			WrongExpected(";", lex);
	}
}

void SyntaxAnalyser::CompStat()
{
	
	auto lex = scanner->NextScan();		// Scan {
	if (lex.type != LexemeType::OpenBrace)
		WrongExpected("{", lex);

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

	std::cout << "----------------------CREATE------------------------------\n";
	semTree->Print();
	node->RightChild.reset();

	std::cout << "------------------------DELETE----------------------------\n";
	semTree->Print();

}

void SyntaxAnalyser::For()
{
	auto lex = scanner->NextScan();		// Scan for
	if (lex.type != LexemeType::For)
		WrongExpected("for", lex);

	lex = scanner->NextScan();					// Scan (
	if (lex.type != LexemeType::OpenPar)
		WrongExpected("(", lex);

	auto node = semTree->AddEmpty();
	semTree->AddScope();
	DataDecl();

	lex = scanner->LookForward(1);
	const auto condType = AssignExpr();
	if (!semTree->CheckCastable(condType, DataType::Bool))
		UncastableError(condType, DataType::Bool, lex);

	lex = scanner->NextScan();					// Scan ;
	if (lex.type != LexemeType::Semi)
		WrongExpected(";", lex);

	AssignExpr();

	lex = scanner->NextScan();					// Scan )
	if (lex.type != LexemeType::ClosePar)
		WrongExpected(")", lex);

	Stat();

	semTree->SetCurrentNode(node);
}

DataType SyntaxAnalyser::AssignExpr()
{
	auto lex = scanner->LookForward(2);
	if (lex.type == LexemeType::Assign)
	{
		lex = scanner->NextScan();									// Scan Id
		if (lex.type != LexemeType::Id)
			WrongId(lex);

		const auto node = semTree->FindNodeUp(lex.str);
		if (node->GetSemanticType() == SemanticType::Empty)					// Check undefined var
			UndefinedError(lex);
		if (node->GetSemanticType() == SemanticType::Func)					// Check assign to func
			AssignToFuncError(lex);


		const auto leftType = node->GetDataType();					// Get data type of var

		lex = scanner->NextScan();									// Scan =

		const auto rightType = EqualExpr();								// Get sem type of right expression

		if (!semTree->CheckCastable(rightType, leftType))	// Check castable right to left
			UncastableError(leftType, rightType, lex);

		semTree->SetVariableInitialized(node);

		return leftType;
	}
	return  EqualExpr();
}

DataType SyntaxAnalyser::EqualExpr()
{
	auto leftType = CmpExpr();										// Get sem type of left expr
	auto lex = scanner->LookForward(1);
	while (lex.type == LexemeType::E || lex.type == LexemeType::NE)
	{
		lex = scanner->NextScan();									// Scan ==, !=
		const auto rightType = CmpExpr();									// Get sem type of right expr

		leftType = CheckOperationResult(leftType, rightType, lex);	// Check operation is valid and get sem type

		lex = scanner->LookForward(1);
	}
	return leftType;
}

DataType SyntaxAnalyser::CmpExpr()
{
	auto leftType = AddExpr();										// Get sem type of left expr
	auto lex = scanner->LookForward(1);
	while (lex.type == LexemeType::G || lex.type == LexemeType::GE
		|| lex.type == LexemeType::L || lex.type == LexemeType::LE)
	{
		lex = scanner->NextScan();									// Scan >, >=, <, <=
		const auto rightType = AddExpr();									// Get sem type of right expr

		leftType = CheckOperationResult(leftType, rightType, lex);	// Check operation is valid and get sem type

		lex = scanner->LookForward(1);
	}
	return leftType;
}

DataType SyntaxAnalyser::AddExpr()
{
	auto leftType = MultExpr();										// Get sem type of left expr
	auto lex = scanner->LookForward(1);
	while (lex.type == LexemeType::Plus
		|| lex.type == LexemeType::Minus)
	{
		scanner->NextScan();										// Scan +, -
		const auto rightType = MultExpr();								// Get sem type of right expr

		leftType = CheckOperationResult(leftType, rightType, lex);	// Check operation is valid and get sem type

		lex = scanner->LookForward(1);
	}
	return leftType;
}

DataType SyntaxAnalyser::MultExpr()
{
	auto leftType = PrefixExpr();									// Get sem type of left expr
	auto lex = scanner->LookForward(1);
	while (lex.type == LexemeType::Mul
		|| lex.type == LexemeType::Div
		|| lex.type == LexemeType::Modul)
	{
		scanner->NextScan();										// Scan *, /, %
		const auto rightType = PrefixExpr();								// Get sem type of right expr

		leftType = CheckOperationResult(leftType, rightType, lex);	// Check operation is valid and get sem type

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
		lex = scanner->NextScan();								// Scan ++, --, +, -
		ops.push(lex);
		lex = scanner->LookForward(1);
	}
	auto type = PostfixExpr();

	while (!ops.empty())
	{
		const auto resType = semTree->GetResultDataType(type, ops.top().type);
		if (resType == DataType::Unknown)
			OperationArgsError(type, ops.top().str, ops.top());
		ops.pop();
		type = resType;
	}
	return type;
}

DataType SyntaxAnalyser::PostfixExpr()
{
	auto lex = scanner->LookForward(1);
	auto lex2 = scanner->LookForward(2);
	if ((lex.type == LexemeType::Id || lex.type == LexemeType::Main)
		&& lex2.type == LexemeType::OpenPar)							// func call
	{
		lex = scanner->NextScan();										// Scan Id, main

		auto funcNode = semTree->FindNodeUp(lex.str);
		if (funcNode->GetSemanticType() == SemanticType::Empty)			// Check defined var
			UndefinedError(lex);
		if (funcNode->GetSemanticType() != SemanticType::Func)			// Check is func
			UseNotFuncError(funcNode->Data->Identifier, lex);

		scanner->NextScan();											// Scan (

		auto paramsTypes = semTree->GetFuncParams(funcNode);	// Get func params types
		size_t argsCount = 0;

		lex = scanner->LookForward(1);
		if (lex.type != LexemeType::ClosePar)
		{
			while (true)
			{
				lex = scanner->LookForward(1);						// Save pos of argument
				auto type = AssignExpr();								// Get type of argument

				if (argsCount < paramsTypes.size() && !semTree->CheckCastable(type, paramsTypes[argsCount]))
					WrongArgType(paramsTypes[argsCount], type, argsCount + 1, lex);

				++argsCount;

				lex = scanner->LookForward(1);
				if (lex.type != LexemeType::Comma)
					break;

				lex = scanner->NextScan();								// Scan ,
			}
		}

		if (argsCount != paramsTypes.size())
			WrongArgsCount(paramsTypes.size(), argsCount, funcNode->Data->Identifier, lex);

		scanner->NextScan();											// Scan )
		if (lex.type != LexemeType::ClosePar)
			WrongExpected(")", lex);

		return DataType::Void;
	}
	
	auto type = PrimExpr();
	lex = scanner->LookForward(1);
	while (lex.type == LexemeType::Inc
		|| lex.type == LexemeType::Dec)
	{
		lex = scanner->NextScan();										// Scan ++, --

		auto resType = semTree->GetResultDataType(type, lex.type);
		if (resType == DataType::Unknown)
			OperationArgsError(type, lex.str, lex);

		type = resType;

		lex = scanner->LookForward(1);
	}
	return type;
}


DataType SyntaxAnalyser::PrimExpr()
{
	auto lex = scanner->NextScan();								// Scan DecNum, HexNum, OctNum, Id, Main (

	if (lex.type == LexemeType::OpenPar)
	{
		const auto resType = AssignExpr();
		lex = scanner->NextScan();
		if (lex.type != LexemeType::ClosePar)
			WrongExpected(")", lex);
		return resType;
	}

	if (lex.type == LexemeType::Id || lex.type == LexemeType::Main) {
		const auto node = semTree->FindNodeUp(lex.str);
		if (node->GetSemanticType() == SemanticType::Empty)
			UndefinedError(lex);
		if (node->GetSemanticType() == SemanticType::Func)
			UseFuncAsVarError(lex);
		if (!semTree->GetVariableInitialized(node))
			VarIsNotInitError(node->Data->Identifier, lex);

		return node->GetDataType();
	}

	if (lex.type == LexemeType::DecimNum || lex.type == LexemeType::HexNum
		|| lex.type == LexemeType::OctNum)
	{
		const auto numType = semTree->GetDataTypeOfNum(lex);
		if (numType == DataType::Unknown)
			WrongNumber(lex);

		return numType;
	}

	ThrowError("Неизвестное выражение: " + lex.str, lex);
}

DataType SyntaxAnalyser::CheckOperationResult(DataType leftType, DataType rightType, const Lexeme& lex) const
{
	const auto resType = semTree->GetResultDataType(leftType, rightType, lex.type);
	if (resType == DataType::Unknown)
		OperationArgsError(leftType, rightType, lex.str, lex);
	return resType;
}


void SyntaxAnalyser::ThrowError(const std::string& mes, const Lexeme& lex)
{
	if (lex.type == LexemeType::End)
		throw SyntaxAnalysisException("Неожиданное обнаружение конца файла", lex.pos);
	throw SyntaxAnalysisException(mes, lex.pos);
}

void SyntaxAnalyser::WrongId(const Lexeme& lex)
{
	ThrowError("Недопустимый идентификатор " + lex.str, lex);
}

void SyntaxAnalyser::WrongType(const Lexeme& lex)
{
	ThrowError("Недопустимый тип: " + lex.str, lex);
}

void SyntaxAnalyser::WrongExpected(const std::string& expected, const Lexeme& lex)
{
	ThrowError("Ожидалось " + expected + ", получено " + lex.str, lex);
}
void SyntaxAnalyser::RedefinitionError(const Lexeme& lex)
{
	ThrowError("Идентификатор \"" + lex.str + "\" уже определен", lex);
}

void SyntaxAnalyser::UndefinedError(const Lexeme& lex)
{
	ThrowError("Идентификатор \"" + lex.str + "\" не определен", lex);
}

void SyntaxAnalyser::UncastableError(DataType from, DataType to, const Lexeme& lex)
{
	ThrowError("Невозможно привести тип " + DataTypeToString(from) + " к типу " + DataTypeToString(to), lex);
}

void SyntaxAnalyser::OperationArgsError(DataType leftType, DataType rightType, const std::string& op,
	const Lexeme& lex)
{
	ThrowError("Невозможно выполнить операцию \"" + op + "\" над типами " + DataTypeToString(leftType) + " и " + DataTypeToString(rightType), lex);
}

void SyntaxAnalyser::OperationArgsError(DataType type, const std::string& op, const Lexeme& lex)
{
	ThrowError("Невозможно выполнить операцию \"" + op + "\" над типом " + DataTypeToString(type), lex);
}

void SyntaxAnalyser::WrongNumber(const Lexeme& lex)
{
	ThrowError("Не удалось определить тип константы", lex);
}

void SyntaxAnalyser::WrongArgsCount(size_t reqCount, size_t givenCount, const std::string& funcId, const Lexeme& lex)
{
	ThrowError("Несоответствие количества параметров и аргументов функции " + funcId
		+ ": требуется " + std::to_string(reqCount) + ", дано " + std::to_string(givenCount), lex);
}

void SyntaxAnalyser::WrongArgType(DataType reqType, DataType givenType, size_t argPos, const Lexeme& lex)
{
	ThrowError("Невозможно привести тип аргумента к параметру функции " + lex.str + " на позиции " + std::to_string(argPos) +
		+": требуется " + DataTypeToString(reqType) + ", дано " + DataTypeToString(givenType), lex);
}

void SyntaxAnalyser::AssignToFuncError(const Lexeme& lex)
{
	ThrowError("Невозможно присвоить значение функции", lex);
}

void SyntaxAnalyser::VarIsNotInitError(const std::string& id, const Lexeme& lex)
{
	ThrowError("Переменная " + id + " не инициализирована перед использованием", lex);
}

void SyntaxAnalyser::UseNotFuncError(const std::string& id, const Lexeme& lex)
{
	ThrowError("Переменная " + id + " не является функцией", lex);
}

void SyntaxAnalyser::UseFuncAsVarError(const Lexeme& lex)
{
	ThrowError("Использование функции в качестве переменной невозможно", lex);
}

bool SyntaxAnalyser::IsType(LexemeType code)
{
	return code == LexemeType::Int || code == LexemeType::Short
		|| code == LexemeType::Long || code == LexemeType::Bool;
}
