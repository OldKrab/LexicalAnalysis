#include <fstream>
#include <sstream>

#include "Scanner.h"

#include <iostream>

std::unordered_map<std::string, LexemeType> Scanner::keywords = {
   {"for", LexemeType::For},
   {"int", LexemeType::Int},
   {"long", LexemeType::Long},
   {"void", LexemeType::Void},
   {"main", LexemeType::Main},
};

Scanner::Scanner(const std::string& sourceFile)
{
	InputSourceText(sourceFile);
	curPos = sourceText.begin();
}



void Scanner::InputSourceText(const std::string& sourceFile)
{
	std::ifstream fin(sourceFile);
	std::stringstream sb;
	sb << fin.rdbuf();
	auto source = sb.str();
	source.push_back(0);
	sourceText = source;
}



Lexeme Scanner::LookForward(int k)
{
	auto savePos = curPos;
	for (int i = 0; i < k - 1; i++)
		NextScan();
	auto lexeme = NextScan();
	curPos = savePos;
	return lexeme;
}

void Scanner::Scan(std::ostream& out)
{
	while (_lexeme.type != LexemeType::End) {
		SourceText::Iterator savePos;
		NextScan();
		out.width(9);
		out.flags(out.left);
		out << _lexeme.str << LexemeTypeToString(_lexeme.type) << " " << _lexeme.pos.row << ' ' << _lexeme.pos.column << std::endl;
	}

}


Lexeme Scanner::NextScan()
{
	_lexeme.str.clear();
	_lexeme.str.reserve(MAX_LEXEME_SIZE);

	SkipIgnoreChars();
	_lexeme.pos = curPos;

	if (*curPos == 0) {
		_lexeme.type = LexemeType::End;
	}

	else if ('a' <= *curPos && *curPos <= 'z' ||
		'A' <= *curPos && *curPos <= 'Z' || '_' == *curPos)
		HandleStringWord();
	else if ('1' <= *curPos && *curPos <= '9')
		HandleDecNum();
	else {
		switch (*curPos)
		{
		case '0':
			HandleHexOrOctNum();
			break;
		case ',':
			NextChar();
			_lexeme.type = LexemeType::Comma;
			break;
		case ';':
			NextChar();
			_lexeme.type = LexemeType::Semi;
			break;
		case '(':
			NextChar();
			_lexeme.type = LexemeType::OpenPar;
			break;
		case ')':
			NextChar();
			_lexeme.type = LexemeType::ClosePar;
			break;
		case '{':
			NextChar();
			_lexeme.type = LexemeType::OpenBrace;
			break;
		case '}':
			NextChar();
			_lexeme.type = LexemeType::CloseBrace;
			break;
		case '*':
			NextChar();
			_lexeme.type = LexemeType::Mul;
			break;
		case '/':
			NextChar();
			_lexeme.type = LexemeType::Div;
			break;
		case '%':
			NextChar();
			_lexeme.type = LexemeType::Modul;
			break;
		case '+':
			HandleDoubleChar(LexemeType::Plus, '+', LexemeType::Inc);
			break;
		case '-':
			HandleDoubleChar(LexemeType::Minus, '-', LexemeType::Dec);
			break;
		case '>':
			HandleDoubleChar(LexemeType::G, '=', LexemeType::GE);
			break;
		case '<':
			HandleDoubleChar(LexemeType::L, '=', LexemeType::LE);
			break;
		case '=':
			HandleDoubleChar(LexemeType::Assign, '=', LexemeType::E);
			break;
		case '!':
			HandleDoubleChar(LexemeType::Err, '=', LexemeType::NE);
			break;
		default:
			NextChar();
			_lexeme.type = LexemeType::Err;
		}
	}

	return _lexeme;
}


void Scanner::SkipIgnoreChars()
{
	while (curPos != sourceText.end())
	{
		switch (*curPos)
		{
		case '/': {
			auto tmpPos = curPos;
			++tmpPos;
			if (*tmpPos != '/')
				return;
			SkipComment();
			break;
		}
		case '\n': case '\r': case '\t': case ' ':
			++curPos;
			break;
		default:
			return;
		}
	}
}

void Scanner::SkipComment()
{
	++curPos;
	while (*curPos != 0 && *curPos != '\n')
		++curPos;
}

void Scanner::HandleStringWord()
{
	NextChar();
	while ('a' <= *curPos && *curPos <= 'z'
		|| 'A' <= *curPos && *curPos <= 'Z'
		|| '0' <= *curPos && *curPos <= '9'
		|| '_' == *curPos)
	{
		if (!NextChar()) {
			HandleErrWord();
			return;
		}
	}
	auto keywordIt = keywords.find(_lexeme.str);
	if (keywordIt != keywords.end())
		_lexeme.type = keywordIt->second;
	else
		_lexeme.type = LexemeType::Id;
}

void Scanner::HandleDecNum()
{
	NextChar();
	while ('0' <= *curPos && *curPos <= '9')
		if (!NextChar())
			return HandleErrWord();
	if (*curPos == 'l' || *curPos == 'L')
		NextChar();
	if ('a' <= *curPos && *curPos <= 'z' || 'A' <= *curPos && *curPos <= 'Z' || '_' == *curPos)
		return HandleErrWord();

	_lexeme.type = LexemeType::DecimNum;
}

void Scanner::HandleHexOrOctNum()
{
	NextChar();
	if (*curPos == 'X' || *curPos == 'x')
		return HandleHexNum();
	return HandleOctNum();
}

void Scanner::HandleHexNum()
{
	NextChar();
	if (!('0' <= *curPos && *curPos <= '9' || 'a' <= *curPos && *curPos <= 'f' || 'A' <= *curPos && *curPos <= 'F'))
		return HandleErrWord();
	while ('0' <= *curPos && *curPos <= '9' || 'a' <= *curPos && *curPos <= 'f' || 'A' <= *curPos && *curPos <= 'F')
		if (!NextChar())
			return HandleErrWord();
	if (*curPos == 'l' || *curPos == 'L')
		NextChar();
	if ('f' < *curPos && *curPos <= 'z' || 'F' < *curPos && *curPos <= 'Z' || '_' == *curPos)
		return HandleErrWord();

	_lexeme.type = LexemeType::HexNum;
}

void Scanner::HandleOctNum()
{
	while ('0' <= *curPos && *curPos <= '7')
		if (!NextChar())
			return HandleErrWord();
	if (*curPos == 'l' || *curPos == 'L')
		NextChar();
	if ('a' <= *curPos && *curPos <= 'z' || 'A' <= *curPos && *curPos <= 'Z' || '8' <= *curPos && *curPos <= '9' || '_' == *curPos)
		return HandleErrWord();

	_lexeme.type = LexemeType::OctNum;
}

void Scanner::HandleErrWord()
{
	while ('a' <= *curPos && *curPos <= 'z' || 'A' <= *curPos && *curPos <= 'Z'
		|| '0' <= *curPos && *curPos <= '9' || '_' == *curPos)
	{
		NextChar();
	}
	_lexeme.type = LexemeType::Err;
}

void Scanner::HandleDoubleChar(LexemeType firstLexeme, char nextChar, LexemeType secondLexeme)
{
	NextChar();
	if (*curPos == nextChar)
	{
		NextChar();
		_lexeme.type = secondLexeme;
	}
	else
		_lexeme.type = firstLexeme;
}

bool Scanner::NextChar()
{
	const bool isLexemeOverflow = _lexeme.str.size() > MAX_LEXEME_SIZE;
	if (!isLexemeOverflow)
		_lexeme.str.push_back(*curPos);
	++curPos;
	return !isLexemeOverflow;
}