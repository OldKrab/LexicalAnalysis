#pragma once
#include <map>
#include <string>

enum class LexemeType : unsigned int
{
	For = 1,
	Int = 2,
	Short = 3,
	Long = 4,
	Bool = 5,
	Main = 6,
	Void = 7,
	Id = 10,
	DecimNum = 20,
	HexNum = 21,
	OctNum = 22,
	Comma = 30,
	Semi = 31,
	OpenPar = 32,
	ClosePar = 33,
	OpenBrace = 34,
	CloseBrace = 35,
	Assign = 40,
	E = 41,
	NE = 42,
	G = 43,
	L = 44,
	LE = 45,
	GE = 46,
	Plus = 47,
	Minus = 48,
	Mul = 49,
	Div = 50,
	Modul = 51,
	Inc = 52,
	Dec = 53,
	End = 100,
	Err = 200
};

inline std::string LexemeTypeToString(LexemeType code) {
	static std::map<LexemeType, std::string> lexicalStrings = {
		{LexemeType::For, "For"},
		{LexemeType::Int, "Int"},
		{LexemeType::Short, "Short"},
		{LexemeType::Long, "Long"},
		{LexemeType::Bool, "Bool"},
		{LexemeType::Main, "Main"},
		{LexemeType::Void, "Void"},
		{LexemeType::Id, "Id"},
		{LexemeType::DecimNum, "DecimNum"},
		{LexemeType::HexNum, "HexNum"},
		{LexemeType::OctNum, "OctNum"},
		{LexemeType::Comma, "Comma"},
		{LexemeType::Semi, "Semi"},
		{LexemeType::OpenPar, "OpenPar"},
		{LexemeType::ClosePar, "ClosePar"},
		{LexemeType::OpenBrace, "OpenBrace"},
		{LexemeType::CloseBrace, "CloseBrace"},
		{LexemeType::Assign, "Assign"},
		{LexemeType::E, "E"},
		{LexemeType::NE, "NE"},
		{LexemeType::G, "G"},
		{LexemeType::L, "L"},
		{LexemeType::LE, "LE"},
		{LexemeType::GE, "GE"},
		{LexemeType::Plus, "Plus"},
		{LexemeType::Minus, "Minus"},
		{LexemeType::Mul, "Mul"},
		{LexemeType::Div, "Div"},
		{LexemeType::Modul, "Modul"},
		{LexemeType::Inc, "Inc"},
		{LexemeType::Dec, "Dec"},
		{LexemeType::End, "End"},
		{LexemeType::Err, "Err"},
	};
	
	return lexicalStrings.at(code);
}



