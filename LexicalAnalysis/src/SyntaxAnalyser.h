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
	void TParams();

	static inline void ThrowError(const std::string& mes, const SourceText::Iterator& pos);
	static bool IsType(LexemeType code);
	Scanner* scanner;
};

inline void SyntaxAnalyser::TProgram()
{
	auto lex = scanner->LookForward(1);
	while (lex.type != LexemeType::End) {
		if (lex.type == LexemeType::Int)
		{
			auto lex2 = scanner->LookForward(2);
			if (lex2.type == LexemeType::Main)
				TFuncDecl();
			else
				TDataDecl();
		}
		else if (IsType(lex.type))
			TFuncDecl();
		else if (lex.type == LexemeType::Void)
			TDataDecl();
		else
			throw SyntaxAnalysisException(std::format("Неизвестный тип \"{}\"", lex.str), lex.pos);
		lex = scanner->LookForward(1);
	}
}

inline void SyntaxAnalyser::TFuncDecl()
{
	auto lex = scanner->NextScan();

	if (lex.type != LexemeType::Void)
		ThrowError("У функции возвращаемый тип должен быть void", lex.pos);
	lex = scanner->NextScan();
	if (lex.type == LexemeType::Id)
	{
		lex = scanner->NextScan();
		if (lex.type != LexemeType::OpenPar)
			ThrowError("Требуется (", lex.pos);
		TParams();
	}
	else if (lex.type == LexemeType::Main)
	{
		lex = scanner->NextScan();
		if (lex.type != LexemeType::OpenPar)
			ThrowError("Требуется (", lex.pos);
	}
	else
		ThrowError("Недопустимый идентификатор", lex.pos);


}

inline void SyntaxAnalyser::TDataDecl()
{
	auto lex = scanner->NextScan();
	if (!IsType(lex.type))
		ThrowError("Недопустимый тип для данных", lex.pos);
	do
	{
		lex = scanner->NextScan();
		if (lex.type != LexemeType::Id)
			ThrowError("Недопустимый идентификатор", lex.pos);
		lex = scanner->NextScan();
		if (lex.type == LexemeType::Assign)
			TAssignExpr();
	} while (lex.type == LexemeType::Comma);
	if (lex.type != LexemeType::Semi)
		ThrowError("Требуется ;", lex.pos);
}

inline void SyntaxAnalyser::TAssignExpr()
{

}

inline void SyntaxAnalyser::TParams()
{

}

inline void SyntaxAnalyser::ThrowError(const std::string& mes, const SourceText::Iterator& pos)
{
	throw SyntaxAnalysisException(mes, pos);
}

inline bool SyntaxAnalyser::IsType(LexemeType code)
{
	return code == LexemeType::Int || code == LexemeType::Short
		|| code == LexemeType::Long || code == LexemeType::Bool;
}

