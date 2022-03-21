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

inline std::shared_ptr<DataValue> GetValueOfVariable(SyntaxAnalyser& sa, std::string id)
{
	auto node = sa.GetSemTree()->FindVariableNodeUp(id);
	return sa.GetSemTree()->GetVariableValue(node);
}



template<class Exception>
void ExpectException(std::string src)
{
	Assert::ExpectException<Exception>([src] {
		RunSyntaxAnalyser(src); });
}
