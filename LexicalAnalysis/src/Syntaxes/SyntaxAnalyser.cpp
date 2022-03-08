#include "SyntaxAnalyser.h"

#include <stack>
#include "SyntaxAnalysisException.h"
#include "Semantics/Node/VarData.h"
#include "Types/SemanticType.h"


void SyntaxAnalyser::Program()
{
	auto lex = scanner->LookForward(1);
	while (lex.type != LexemeType::End) {
		if (IsDataType(lex.type))
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
	if (!IsDataType(lex.type))
		WrongType(lex);

	const auto leftType = LexemeStringToDataType(lex.str);				// Get data type
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
			const auto rightValue = AssignExpr();								// Get data type of right expression

			if (!semTree->CheckCastable(rightValue.type, leftType))	// Check castable
				UncastableError(rightValue.type, leftType, lex);

			semTree->SetVariableValue(varNode, rightValue);

			lex = scanner->NextScan();									//Scan  ',', ';'
		}

	} while (lex.type == LexemeType::Comma);

	if (lex.type != LexemeType::Semi)
		WrongExpected(";", lex);
}



void SyntaxAnalyser::Params(Node* funcNode) const
{
	auto lex = scanner->LookForward(1);
	if (!IsDataType(lex.type))
		return;

	while (true) {
		lex = scanner->NextScan();						// Scan Type
		if (!IsDataType(lex.type))
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



	node->RightChild.reset();					// Delete subtree
}

void SyntaxAnalyser::For()
{
	auto lex = scanner->NextScan();		// Scan for
	if (lex.type != LexemeType::For)
		WrongExpected("for", lex);

	lex = scanner->NextScan();					// Scan (
	if (lex.type != LexemeType::OpenPar)
		WrongExpected("(", lex);

	auto node = semTree->AddEmpty();		// Add empty node, save it
	semTree->AddScope();
	DataDecl();

	lex = scanner->LookForward(1);
	const auto condValue = AssignExpr();
	if (!semTree->CheckCastable(condValue.type, DataType::Int))
		UncastableError(condValue.type, DataType::Int, lex);

	lex = scanner->NextScan();					// Scan ;
	if (lex.type != LexemeType::Semi)
		WrongExpected(";", lex);

	AssignExpr();

	lex = scanner->NextScan();					// Scan )
	if (lex.type != LexemeType::ClosePar)
		WrongExpected(")", lex);

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
			WrongId(lex);

		const auto node = semTree->FindNodeUp(lex.str);
		if (node->GetSemanticType() == SemanticType::Empty)						// Check undefined var
			UndefinedError(lex);
		if (node->GetSemanticType() == SemanticType::Func)						// Check assign to func
			AssignToFuncError(lex);

		lex = scanner->NextScan();												// Scan =

		const auto leftValue = semTree->GetVariableValue(node);					// Get value of expressions
		auto rightValue = EqualExpr();

		if (!semTree->CheckCastable(rightValue.type, leftValue.type))	// Check castable right to left
			UncastableError(leftValue.type, rightValue.type, lex);

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
		UndefinedError(lex);
	if (funcNode->GetSemanticType() != SemanticType::Func)			// Check is func
		UseNotFuncError(funcNode->Data->Identifier, lex);

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
				WrongArgType(paramsTypes[argsCount], value.type, argsCount + 1, lex);

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
}


DataValue SyntaxAnalyser::PrimExpr()
{
	auto lex = scanner->NextScan();								// Scan DecNum, HexNum, OctNum, Id, Main (

	if (lex.type == LexemeType::OpenPar)								// (expr)
	{
		const auto resValue = AssignExpr();
		lex = scanner->NextScan();
		if (lex.type != LexemeType::ClosePar)
			WrongExpected(")", lex);
		return resValue;
	}

	if (lex.type == LexemeType::Id || lex.type == LexemeType::Main)		// identifier
	{
		const auto node = semTree->FindNodeUp(lex.str);
		if (node->GetSemanticType() == SemanticType::Empty)
			UndefinedError(lex);
		if (node->GetSemanticType() == SemanticType::Func)
			UseFuncAsVarError(lex);
		if (!semTree->GetVariableInitialized(node))
			VarIsNotInitError(node->Data->Identifier, lex);

		return semTree->GetVariableValue(node);
	}

	if (lex.type == LexemeType::DecimNum || lex.type == LexemeType::HexNum
		|| lex.type == LexemeType::OctNum)
	{
		const auto numValue = semTree->GetValueOfNum(lex);
		if (numValue.type == DataType::Unknown)
			WrongNumber(lex);
		return numValue;
	}

	ThrowError("Неизвестное выражение: " + lex.str, lex);
}

DataType SyntaxAnalyser::CheckOperationValid(DataValue leftValue, DataValue rightValue, const Lexeme& lex) const
{
	const auto resType = semTree->GetResultDataType(leftValue.type, rightValue.type, lex.type);
	if (resType == DataType::Unknown)
		OperationArgsError(leftValue.type, rightValue.type, lex.str, lex);
	if ((lex.type == LexemeType::Div || lex.type == LexemeType::Modul)
		&& (rightValue.type == DataType::Long && rightValue.longVal == 0
			|| rightValue.type == DataType::Int && rightValue.intVal == 0))
		DivisionOnZero(lex);
	return resType;
}


DataType SyntaxAnalyser::CheckOperationValid(DataType type, const Lexeme& lex) const
{
	if (type == DataType::Unknown)
		OperationArgsError(type, lex.str, lex);
	return type;
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

void SyntaxAnalyser::DivisionOnZero(const Lexeme& lex)
{
	ThrowError("Деление на ноль!", lex);
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

bool SyntaxAnalyser::IsDataType(LexemeType code)
{
	return code == LexemeType::Int || code == LexemeType::Long;
}
