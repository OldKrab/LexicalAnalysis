#pragma once
#include <sstream>

#include "Syntaxes/SyntaxAnalyser.h"

inline SyntaxAnalyser RunSyntaxAnalyser(std::string src)
{
	std::stringstream ss(src);
	SyntaxAnalyser sa(ss);
	sa.Program();
	return sa;
}

template<class Exception>
void ExpectException(std::string src)
{
	Assert::ExpectException<Exception>([src] {
		RunSyntaxAnalyser(src); });
}
