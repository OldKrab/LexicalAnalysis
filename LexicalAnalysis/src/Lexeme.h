#pragma once
#include <string>

#include "Types.h"
#include "SourceText.h"

struct Lexeme
{
	Lexeme() :type(LexemeType::Err) {}
	std::string str;
	LexemeType type;
	SourceText::Iterator pos;
};

