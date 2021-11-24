#include "SyntaxAnalyser.h"

#include <iostream>
#include <SyntaxAnalysisException.h>


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
	semTree->Print(std::cout);
}

void SyntaxAnalyser::FuncDecl()
{
	auto lex = scanner->NextScan();			// Void
	if (lex.type != LexemeType::Void)
		WrongExpected("void", lex);

	lex = scanner->NextScan();						// Id, Main

	auto funcNode = semTree->AddFunc(lex.str);

	if (lex.type == LexemeType::Id)
	{
		lex = scanner->NextScan();					// (
		if (lex.type != LexemeType::OpenPar)
			WrongExpected("(", lex);
		Params(funcNode);
	}
	else if (lex.type == LexemeType::Main)
	{
		lex = scanner->NextScan();					// (
		if (lex.type != LexemeType::OpenPar)
			WrongExpected("(", lex);
	}
	else
		WrongId(lex);
	lex = scanner->NextScan();						// )
	if (lex.type != LexemeType::ClosePar)
		WrongExpected(")", lex);



	CompStat();
}

void SyntaxAnalyser::DataDecl()
{
	auto lex = scanner->NextScan();		// Type
	if (!IsType(lex.type))
		WrongType(lex);
	do
	{
		lex = scanner->NextScan();				// Id
		if (lex.type != LexemeType::Id)
			WrongId(lex);
		lex = scanner->NextScan();				// Assign, Comma, Semi
		if (lex.type == LexemeType::Assign) {
			AssignExpr();
			lex = scanner->NextScan();				//  Comma, Semi
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
		lex = scanner->NextScan();				// Type
		if (!IsType(lex.type))
			WrongType(lex);

		auto type = lex.type;

		lex = scanner->NextScan();				// Id
		if (lex.type != LexemeType::Id)
			WrongId(lex);

		semTree->AddParam(funcNode, lex.str, type);

		lex = scanner->LookForward(1);
		if (lex.type != LexemeType::Comma)
			return;
		lex = scanner->NextScan();				// Comma
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
		if (lex.type != LexemeType::Semi)
			AssignExpr();
		lex = scanner->NextScan();				// ;
		if (lex.type != LexemeType::Semi)
			WrongExpected(";", lex);
	}
}

void SyntaxAnalyser::CompStat()
{
	auto lex = scanner->NextScan();		// {
	if (lex.type != LexemeType::OpenBrace)
		WrongExpected("{", lex);

	semTree->AddScope();

	lex = scanner->LookForward(1);
	while (lex.type != LexemeType::CloseBrace)
	{
		Stat();
		lex = scanner->LookForward(1);
	}
	lex = scanner->NextScan();					// }
}

void SyntaxAnalyser::For()
{
	auto lex = scanner->NextScan();		// for
	if (lex.type != LexemeType::For)
		WrongExpected("for", lex);
	lex = scanner->NextScan();					// (
	if (lex.type != LexemeType::OpenPar)
		WrongExpected("(", lex);
	DataDecl();
	AssignExpr();
	lex = scanner->NextScan();					// ;
	if (lex.type != LexemeType::Semi)
		WrongExpected(";", lex);
	AssignExpr();
	lex = scanner->NextScan();					// )
	if (lex.type != LexemeType::ClosePar)
		WrongExpected(")", lex);
	Stat();
}

void SyntaxAnalyser::AssignExpr()
{
	auto lex = scanner->LookForward(2);
	if (lex.type == LexemeType::Assign)
	{
		lex = scanner->NextScan();				// Id
		if (lex.type != LexemeType::Id)
			WrongId(lex);
		lex = scanner->NextScan();				// =
	}
	EqualExpr();
}

void SyntaxAnalyser::EqualExpr()
{
	CmpExpr();
	auto lex = scanner->LookForward(1);
	while (lex.type == LexemeType::E || lex.type == LexemeType::NE)
	{
		scanner->NextScan();					// ==, !=
		CmpExpr();
		lex = scanner->LookForward(1);
	}
}

void SyntaxAnalyser::CmpExpr()
{
	AddExpr();
	auto lex = scanner->LookForward(1);
	while (lex.type == LexemeType::G || lex.type == LexemeType::GE
		|| lex.type == LexemeType::L || lex.type == LexemeType::LE)
	{
		scanner->NextScan();					// >, >=, <, <=
		AddExpr();
		lex = scanner->LookForward(1);
	}
}

void SyntaxAnalyser::AddExpr()
{
	MultExpr();
	auto lex = scanner->LookForward(1);
	while (lex.type == LexemeType::Plus || lex.type == LexemeType::Minus)
	{
		scanner->NextScan();					// +, -
		MultExpr();
		lex = scanner->LookForward(1);
	}
}

void SyntaxAnalyser::MultExpr()
{
	PrefixExpr();
	auto lex = scanner->LookForward(1);
	while (lex.type == LexemeType::Mul || lex.type == LexemeType::Div || lex.type == LexemeType::Modul)
	{
		scanner->NextScan();					// *, /, %
		PrefixExpr();
		lex = scanner->LookForward(1);
	}
}

void SyntaxAnalyser::PrefixExpr()
{
	auto lex = scanner->LookForward(1);
	while (lex.type == LexemeType::Inc || lex.type == LexemeType::Dec
		|| lex.type == LexemeType::Plus || lex.type == LexemeType::Minus)
	{
		scanner->NextScan();					// ++, --, +, -
		lex = scanner->LookForward(1);
	}
	PostfixExpr();
}

void SyntaxAnalyser::PostfixExpr()
{
	auto lex = scanner->LookForward(1);
	auto lex2 = scanner->LookForward(2);
	if (lex.type == LexemeType::Id && lex2.type == LexemeType::OpenPar)
	{
		lex = scanner->NextScan();					// Id, main
		if (lex.type != LexemeType::Id && lex.type != LexemeType::Main)
			WrongId(lex);
		scanner->NextScan();						// (
		lex = scanner->LookForward(1);
		if (lex.type != LexemeType::ClosePar) {
			while (true) {
				AssignExpr();
				lex = scanner->LookForward(1);
				if (lex.type != LexemeType::Comma)
					break;
				lex = scanner->NextScan();			// Comma
			}
		}
		scanner->NextScan();						// )
		if (lex.type != LexemeType::ClosePar)
			WrongExpected(")", lex);
	}
	else
	{
		PrimExpr();
		lex = scanner->LookForward(1);
		while (lex.type == LexemeType::Inc || lex.type == LexemeType::Dec)
		{
			scanner->NextScan();					// ++, --
			lex = scanner->LookForward(1);
		}
	}
}


void SyntaxAnalyser::PrimExpr()
{
	auto lex = scanner->NextScan();			// DecNum, HexNum, OctNum, {, Id
	if (lex.type == LexemeType::OpenPar)
	{
		AssignExpr();
		lex = scanner->NextScan();
		if (lex.type != LexemeType::ClosePar)
			WrongExpected(")", lex);
	}
	else if (lex.type != LexemeType::DecimNum && lex.type != LexemeType::HexNum
		&& lex.type != LexemeType::OctNum && lex.type != LexemeType::Id && lex.type != LexemeType::Main)
	{
		ThrowError("Неизвестное выражение: " + lex.str, lex);
	}
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

bool SyntaxAnalyser::IsType(LexemeType code)
{
	return code == LexemeType::Int || code == LexemeType::Short
		|| code == LexemeType::Long || code == LexemeType::Bool;
}
