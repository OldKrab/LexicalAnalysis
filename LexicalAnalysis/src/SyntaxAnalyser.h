#pragma once
#include "Scanner.h"
#include "SyntaxAnalysisException.h"
#include <format>


class SyntaxAnalyser
{
public:
	SyntaxAnalyser(Scanner* scanner) :scanner(scanner) {}

	inline void TProgram();
private:
	void TFuncDecl();
	void TDataDecl();
	void TAssignExpr();

	static inline void ThrowError(const std::string& mes, const SourceText::Iterator& pos);
	static bool IsType(LexemeCode code);
	Scanner* scanner;
};

inline void SyntaxAnalyser::TProgram()
{
	auto lex = scanner->LookForward(1);
	while (lex.code != LexemeCode::TEnd) {
		if (lex.code == LexemeCode::TInt)
		{
			auto lex2 = scanner->LookForward(2);
			if (lex2.code == LexemeCode::TMain)
				TFuncDecl();
			else
				TDataDecl();
		}
		else if (IsType(lex.code))
			TFuncDecl();
		else if (lex.code == LexemeCode::TVoid)
			TDataDecl();
		else
			throw SyntaxAnalysisException(std::format("Неизвестный тип \"{}\"", lex.str), lex.pos);
		lex = scanner->LookForward(1);
	}
}

inline void SyntaxAnalyser::TFuncDecl()
{

}

inline void SyntaxAnalyser::TDataDecl()
{
	auto lex = scanner->NextScan();
	if (!IsType(lex.code))
		ThrowError("Недопустимый тип для данных", lex.pos);
	do
	{
		lex = scanner->NextScan();
		if (lex.code != LexemeCode::TId)
			ThrowError("Недопустимый идентификатор", lex.pos);
		lex = scanner->NextScan();
		if (lex.code == LexemeCode::TAssign)
			TAssignExpr();
	} while (lex.code == LexemeCode::TComma);
	if(lex.code != LexemeCode::TSemi)
		ThrowError("Требуется ;", lex.pos);
}

inline void SyntaxAnalyser::TAssignExpr()
{
}

inline void SyntaxAnalyser::ThrowError(const std::string& mes, const SourceText::Iterator& pos)
{
	throw SyntaxAnalysisException(mes, pos);
}

inline bool SyntaxAnalyser::IsType(LexemeCode code)
{
	return code == LexemeCode::TInt || code == LexemeCode::TShort
		|| code == LexemeCode::TLong || code == LexemeCode::TBool;
}

