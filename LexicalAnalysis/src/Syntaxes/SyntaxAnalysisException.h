#pragma once
#include <exception>
#include <sstream>
#include "Lexical/SourceText.h"
class SyntaxAnalysisException : public std::exception
{
public:
	SyntaxAnalysisException(std::string message, SourceText::Iterator pos)
	{
		std::stringstream ss;
		ss << "(" << pos.row << ", " << pos.column << "): error: " << message;
		this->message = ss.str();
	}
	char const* what() const override;
private:
	std::string message;
};

inline char const* SyntaxAnalysisException::what() const
{
	return message.c_str();
}
