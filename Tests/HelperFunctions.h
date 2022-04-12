#pragma once
#include <sstream>

#include "Syntaxes/SyntaxAnalyser.h"

inline SyntaxAnalyser RunSyntaxAnalyser(const std::string& src)
{
	std::stringstream ss(src);
	SyntaxAnalyser sa(ss);
	sa.Program();
	return sa;
}

template<class Exception>
void ExpectException(const std::string& src)
{
	Microsoft::VisualStudio::CppUnitTestFramework::Assert::ExpectException<Exception>([src] {
		RunSyntaxAnalyser(src); });
}
