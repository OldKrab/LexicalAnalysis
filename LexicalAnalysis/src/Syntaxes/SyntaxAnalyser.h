#pragma once
#include "IntermediateCode/IntermediateGenerator.h"
#include "IntermediateCode/Triad.h"
#include "Lexical/Scanner.h"
#include "Semantics/SemanticTree.h"

class SyntaxAnalyser
{
public:
	SyntaxAnalyser(const std::istream& srcStream)
		: scanner(std::make_unique<Scanner>(srcStream)),
		interGen(std::make_unique<IntermediateGenerator>())
	{
		semTree = std::make_unique<SemanticTree>(interGen.get());
	}
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


	std::tuple<Operand, DataType> FuncCall();
	std::tuple<Operand, DataType> AssignExpr();
	std::tuple<Operand, DataType> EqualExpr();
	std::tuple<Operand, DataType> CmpExpr();
	std::tuple<Operand, DataType> AddExpr();
	std::tuple<Operand, DataType> MultExpr();
	std::tuple<Operand, DataType> PrefixExpr();
	std::tuple<Operand, DataType> PostfixExpr();
	std::tuple<Operand, DataType> PrimExpr();


	static void CheckExpectedLexeme(const Lexeme& givenLexeme, LexemeType expected);
	bool IsTypeForward(LexemeType type, int distance = 1) const;
	static bool IsDataType(LexemeType code);


	std::unique_ptr<Scanner> scanner;
	std::unique_ptr<SemanticTree> semTree;
	std::unique_ptr<IntermediateGenerator> interGen;
};


