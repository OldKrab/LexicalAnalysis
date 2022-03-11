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
	void PrintAnalysis();

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


	static void CheckExpectedLexeme(const Lexeme& givenLexeme, LexemeType expected);
	 bool IsTypeForward(LexemeType type, int distance = 1) const;
	static bool IsDataType(LexemeType code);


	std::unique_ptr<Scanner> scanner;
	std::unique_ptr<SemanticTree> semTree;
};


