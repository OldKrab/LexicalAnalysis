#pragma once
#include "Interpretator/DataValue.h"
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
private:
	void FuncDecl();
	void DataDecl();
	void Params(const Node* funcNode) const;
	void Stat();
	void CompStat();
	void For();
	void FuncCall();


	std::shared_ptr<DataValue> AssignExpr();
	std::shared_ptr<DataValue> EqualExpr();
	std::shared_ptr<DataValue> CmpExpr();
	std::shared_ptr<DataValue> AddExpr();
	std::shared_ptr<DataValue> MultExpr();
	std::shared_ptr<DataValue> PrefixExpr();
	std::shared_ptr<DataValue> PostfixExpr();
	std::shared_ptr<DataValue> PrimExpr();


	static void CheckExpectedLexeme(const Lexeme& givenLexeme, LexemeType expected);
	 bool IsTypeForward(LexemeType type, int distance = 1) const;
	static bool IsDataType(LexemeType code);


	std::unique_ptr<Scanner> scanner;
	std::unique_ptr<SemanticTree> semTree;
};


