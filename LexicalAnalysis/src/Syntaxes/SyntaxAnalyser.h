#pragma once
#include "Interpretator/DataValue.h"
#include "Lexical/Scanner.h"
#include "Semantics/SemanticTree.h"

class SyntaxAnalyser
{
public:
	SyntaxAnalyser(const std::string& filePath)
		: scanner(std::make_unique<Scanner>(filePath)),
		semTree(std::make_unique<SemanticTree>())
	{}

	void Program();

private:
	void FuncDecl();
	void DataDecl();
	void Params(Node* funcNode) const;
	void Stat();
	void CompStat();
	void For();
	void FuncCall();


	DataValue AssignExpr();
	DataValue EqualExpr();
	DataValue CmpExpr();
	DataValue AddExpr();
	DataValue MultExpr();
	DataValue PrefixExpr();
	DataValue PostfixExpr();
	DataValue PrimExpr();

	DataType CheckOperationValid(DataValue leftValue, DataValue rightValue, const Lexeme& lex) const;
	DataType CheckOperationValid(DataType type, const Lexeme& lex) const;

	static bool IsDataType(LexemeType code);


	// Errors functions
	[[noreturn]] static void ThrowError(const std::string& mes, const Lexeme& lex);
	[[noreturn]] static void WrongId(const Lexeme& lex);
	[[noreturn]] static void WrongType(const Lexeme& lex);
	[[noreturn]] static void WrongExpected(const std::string& expected, const Lexeme& lex);
	[[noreturn]] static void RedefinitionError(const Lexeme& lex);
	[[noreturn]] static void UndefinedError(const Lexeme& lex);
	[[noreturn]] static void UncastableError(DataType from, DataType to, const Lexeme& lex);
	[[noreturn]] static void OperationArgsError(DataType leftType, DataType rightType, const std::string& op, const Lexeme& lex);
	[[noreturn]] static void DivisionOnZero(const Lexeme& lex);
	[[noreturn]] static void OperationArgsError(DataType type, const std::string& op, const Lexeme& lex);
	[[noreturn]] static void WrongNumber(const Lexeme& lex);
	[[noreturn]] static void WrongArgsCount(size_t reqCount, size_t givenCount, const std::string& funcId, const Lexeme& lex);
	[[noreturn]] static void WrongArgType(DataType reqType, DataType givenType, size_t argPos, const Lexeme& lex);
	[[noreturn]] static void AssignToFuncError(const Lexeme& lex);
	[[noreturn]] static void VarIsNotInitError(const std::string& id, const Lexeme& lex);
	[[noreturn]] static void UseNotFuncError(const std::string& id, const Lexeme& lex);
	[[noreturn]] static void UseFuncAsVarError(const Lexeme& lex);


	std::unique_ptr<Scanner> scanner;
	std::unique_ptr<SemanticTree> semTree;
};


