#pragma once
#include <string>

#include "SourceText.h"
#include "Types/LexemeType.h"

struct Lexeme
{
	Lexeme() :type(LexemeType::Err) {}
	std::string str;
	LexemeType type;
	SourceText::Iterator pos;
};

