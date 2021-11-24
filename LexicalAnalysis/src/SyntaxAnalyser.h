#pragma once
#include "Scanner.h"
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
	void AssignExpr();
	void EqualExpr();
	void CmpExpr();
	void AddExpr();
	void MultExpr();
	void PrefixExpr();
	void PostfixExpr();
	void PrimExpr();

	static void ThrowError(const std::string& mes, const Lexeme& lex);
	static void WrongId(const Lexeme& lex);
	static void WrongType(const Lexeme& lex);
	static void WrongExpected(const std::string& expected, const Lexeme& lex);
	static bool IsType(LexemeType code);

	std::unique_ptr<Scanner> scanner;
	std::unique_ptr<SemanticTree> semTree;
};


