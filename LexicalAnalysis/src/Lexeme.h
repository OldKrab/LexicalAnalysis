#pragma once
#include <string>

#include "LexemeCode.h"
#include "SourceText.h"

struct Lexeme
{
	Lexeme() :code(LexemeCode::TErr) {}
	std::string str;
	LexemeCode code;
	SourceText::Iterator pos;
};
