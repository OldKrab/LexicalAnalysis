#pragma once
#include <exception>
#include <sstream>

#include "Lexical/Lexeme.h"

class AnalysisException : public std::exception
{
public:
	char const* what() const override
	{
		return message.c_str();
	}

protected:
	explicit AnalysisException() = default;

	std::string message;
};

// Syntax Exceptions

class SyntaxException : public AnalysisException {
public:
	char const* what() const override
	{
		static auto resMessage = "Синтаксическая ошибка: " + message;
		return resMessage.c_str();
	}
};

class InvalidIdentifierException : public SyntaxException
{
public:
	InvalidIdentifierException(const std::string& id)
	{
		message = "Недопустимый идентификатор " + id;
	}
};

class InvalidTypeException : public SyntaxException
{
public:
	InvalidTypeException(const std::string& type)
	{
		message = "Недопустимый тип " + type;
	}
};

class NotExpectedLexemeException : public SyntaxException
{
public:
	NotExpectedLexemeException(const std::string& expected, const Lexeme& resultLexeme)
	{
		if (resultLexeme.type == LexemeType::End)
			message = "Неожиданное завершение файла";
		else
			message = "Ожидалось " + expected + ", получено" + resultLexeme.str;
	}
};

class UnknownLexemeException : public SyntaxException
{
public:
	UnknownLexemeException(const Lexeme& lexeme)
	{
		message = "Неизвестное выражение: " + lexeme.str;
	}
};



// Semantic Exceptions

class SemanticException : public AnalysisException {
public:
	char const* what() const override
	{
		static auto resMessage = "Семантическая ошибка: " + message;
		return resMessage.c_str();
	}
};

class RedefinedIdentifierException : public SemanticException
{
public:
	RedefinedIdentifierException(const std::string& id)
	{
		message = "Идентификатор \"" + id + "\" уже определен";
	}
};

class UndefinedIdentifierException : public SemanticException
{
public:
	UndefinedIdentifierException(const std::string& id)
	{
		message = "Идентификатор \"" + id + "\" не определен";
	}
};

class UncastableVariableException : public SemanticException
{
public:
	UncastableVariableException(DataType from, DataType to)
	{
		message = "Невозможно привести тип " + DataTypeToString(from) + " к типу " + DataTypeToString(to);
	}
};

class InvalidOperandsException : public SemanticException
{
public:
	InvalidOperandsException(DataType leftType, DataType rightType, const std::string& binaryOp)
	{
		message = "Невозможно выполнить бинарную операцию \"" + binaryOp + "\" над типами " + DataTypeToString(leftType) + " и " + DataTypeToString(rightType);
	}

	InvalidOperandsException(DataType type, const std::string& unaryOp)
	{
		message = "Невозможно выполнить унарную операцию \"" + unaryOp + "\" над типом " + DataTypeToString(type);
	}
};

class DivisionOnZeroException : public SemanticException
{
public:
	DivisionOnZeroException()
	{
		message = "Деление на ноль!";
	}
};

class InvalidNumberException : public SemanticException
{
public:
	InvalidNumberException()
	{
		message = "Не удалось определить тип константы";
	}
};

class WrongArgsCountException : public SemanticException
{
public:
	WrongArgsCountException(size_t reqCount, size_t givenCount, const std::string& funcId)
	{
		message = "Несоответствие количества параметров и аргументов функции " + funcId
			+ ": требуется " + std::to_string(reqCount) + ", дано " + std::to_string(givenCount);
	}
};


class UsingUninitializedVariableException : public SemanticException
{
public:
	UsingUninitializedVariableException(const std::string& id)
	{
		message = "Переменная " + id + " не инициализирована перед использованием";
	}
};

class UsingVariableAsFunctionException : public SemanticException
{
public:
	UsingVariableAsFunctionException(const std::string& id)
	{
		message = "Переменная " + id + " не является функцией";
	}
};

class UsingFunctionAsVariableException : public SemanticException
{
public:
	UsingFunctionAsVariableException(const std::string& id)
	{
		message = "Функция " + id + "vне может использоваться как переменная";
	}
};