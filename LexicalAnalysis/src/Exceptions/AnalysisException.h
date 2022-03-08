#pragma once
#include <exception>
#include <sstream>
#include "Lexical/SourceText.h"
class AnalysisException : public std::exception
{
public:
	AnalysisException(std::string message, SourceText::Iterator pos)
	{
		std::stringstream ss;
		ss << "(" << pos.row << ", " << pos.column << "): " << message;
		this->message = ss.str();
	}
	char const* what() const override
	{
		return message.c_str();
	}
private:
	std::string message;
};


