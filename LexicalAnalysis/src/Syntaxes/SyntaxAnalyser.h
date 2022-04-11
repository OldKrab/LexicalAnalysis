#pragma once
#include "Lexical/Scanner.h"
#include "Semantics/SemanticTree.h"

class SyntaxAnalyser
{
public:
	SyntaxAnalyser(const std::istream& srcStream)
		: scanner(std::make_unique<Scanner>(srcStream)),
		semTree(std::make_unique<SemanticTree>())
	{}
	void PrintAnalysis();

	void Program();

	SemanticTree* GetSemTree() const { return semTree.get(); }
private:
	void FuncDecl();
	void DataDecl();
	void Params(const Node* funcNode) const;
	void Stat();
	void CompStat();
	void For();


	DataType FuncCall();
	DataType AssignExpr();
	DataType EqualExpr();
	DataType CmpExpr();
	DataType AddExpr();
	DataType MultExpr();
	DataType PrefixExpr();
	DataType PostfixExpr();
	DataType PrimExpr();


	static void CheckExpectedLexeme(const Lexeme& givenLexeme, LexemeType expected);
	bool IsTypeForward(LexemeType type, int distance = 1) const;
	static bool IsDataType(LexemeType code);


	std::unique_ptr<Scanner> scanner;
	std::unique_ptr<SemanticTree> semTree;
};


