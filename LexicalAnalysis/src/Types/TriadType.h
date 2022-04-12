#pragma once
#pragma once
#include <map>
#include <string>

#include "LexemeType.h"

enum class TriadType : unsigned int
{
	Call,
	Assign,
	E,
	NE,
	G,
	L,
	LE,
	GE,
	Plus,
	Minus,
	Mul,
	Div,
	Modul,
	Inc,
	Dec,
};

inline std::string TriadTypeToString(TriadType code) {
	static std::map<TriadType, std::string> triadStrings = {
		{TriadType::Assign, "Assign"},
		{TriadType::E, "E"},
		{TriadType::NE, "NE"},
		{TriadType::G, "G"},
		{TriadType::L, "L"},
		{TriadType::LE, "LE"},
		{TriadType::GE, "GE"},
		{TriadType::Plus, "Plus"},
		{TriadType::Minus, "Minus"},
		{TriadType::Mul, "Mul"},
		{TriadType::Div, "Div"},
		{TriadType::Modul, "Modul"},
		{TriadType::Inc, "Inc"},
		{TriadType::Dec, "Dec"},
		{TriadType::Call, "Call"}
	};

	return triadStrings.at(code);
}



inline TriadType LexemeTypeToTriadType(LexemeType type) {
	static std::map<LexemeType, TriadType> map = {
		{LexemeType::Assign, TriadType::Assign},
		{LexemeType::E, TriadType::E},
		{LexemeType::NE, TriadType::NE},
		{LexemeType::G, TriadType::G},
		{LexemeType::L, TriadType::L},
		{LexemeType::LE, TriadType::LE},
		{LexemeType::GE, TriadType::GE},
		{LexemeType::Plus, TriadType::Plus},
		{LexemeType::Minus, TriadType::Minus},
		{LexemeType::Mul, TriadType::Mul},
		{LexemeType::Div, TriadType::Div},
		{LexemeType::Modul, TriadType::Modul},
		{LexemeType::Inc, TriadType::Inc},
		{LexemeType::Dec, TriadType::Dec}
	};

	return map.at(type);
}


