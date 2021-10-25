#pragma once
#include <iomanip>
#include <unordered_map>
#include <map>
#include <string>

#include "Lexeme.h"
#include "SourceText.h"


class Scanner
{
public:
	Scanner(const std::string& sourceFile);
	inline void Scan(std::ostream& out);
	inline Lexeme NextScan();
	inline Lexeme LookForward(int k);
private:
	inline void SkipIgnoreChars();
	inline void SkipComment();

	inline void HandleStringWord();
	inline void HandleDecNum();
	inline void HandleHexOrOctNum();
	inline void HandleHexNum();
	inline void HandleOctNum();
	inline void HandleErrWord();
	inline void HandleDoubleChar(LexemeCode firstLexeme, char nextChar, LexemeCode secondLexeme);

	inline bool NextChar();
	void InputSourceText(const std::string& sourceFile);

	SourceText sourceText;
	SourceText::Iterator curPos;
	Lexeme _lexeme;

	static std::unordered_map<std::string, LexemeCode> keywords;
	static const int MAX_LEXEME_SIZE = 100;
};

inline Lexeme Scanner::LookForward(int k)
{
	auto savePos = curPos;
	for(int i = 0; i < k - 1; i++)
		NextScan();
	auto lexeme = NextScan();
	curPos = savePos;
	return lexeme;
}

inline void Scanner::Scan(std::ostream& out)
{
	while (_lexeme.code != LexemeCode::TEnd) {
		SourceText::Iterator savePos;
		NextScan();
		out.width(9);
		out.flags(out.left);
		out << _lexeme.str << LexicalCodeToString(_lexeme.code) << " " << _lexeme.pos.row << ' ' << _lexeme.pos.column << std::endl;
	}

}


inline Lexeme Scanner::NextScan()
{
	_lexeme.str.clear();
	_lexeme.str.reserve(MAX_LEXEME_SIZE);

	SkipIgnoreChars();
	_lexeme.pos = curPos;

	if (*curPos == 0) {
		_lexeme.code = LexemeCode::TEnd;
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
			_lexeme.code = LexemeCode::TComma;
			break;
		case ';':
			NextChar();
			_lexeme.code = LexemeCode::TSemi;
			break;
		case '(':
			NextChar();
			_lexeme.code = LexemeCode::TOpenPar;
			break;
		case ')':
			NextChar();
			_lexeme.code = LexemeCode::TClosePar;
			break;
		case '{':
			NextChar();
			_lexeme.code = LexemeCode::TOpenBrace;
			break;
		case '}':
			NextChar();
			_lexeme.code = LexemeCode::TCloseBrace;
			break;
		case '*':
			NextChar();
			_lexeme.code = LexemeCode::TMul;
			break;
		case '/':
			NextChar();
			_lexeme.code = LexemeCode::TDiv;
			break;
		case '%':
			NextChar();
			_lexeme.code = LexemeCode::TModul;
			break;
		case '+':
			HandleDoubleChar(LexemeCode::TPlus, '+', LexemeCode::TInc);
			break;
		case '-':
			HandleDoubleChar(LexemeCode::TMinus, '-', LexemeCode::TDec);
			break;
		case '>':
			HandleDoubleChar(LexemeCode::TG, '=', LexemeCode::TGE);
			break;
		case '<':
			HandleDoubleChar(LexemeCode::TL, '=', LexemeCode::TLE);
			break;
		case '=':
			HandleDoubleChar(LexemeCode::TAssign, '=', LexemeCode::TE);
			break;
		case '!':
			HandleDoubleChar(LexemeCode::TErr, '=', LexemeCode::TNE);
			break;
		default:
			NextChar();
			_lexeme.code = LexemeCode::TErr;
		}
	}
	return _lexeme;
}


inline void Scanner::SkipIgnoreChars()
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

inline void Scanner::SkipComment()
{
	++curPos;
	while (*curPos != 0 && *curPos != '\n')
		++curPos;
}

inline void Scanner::HandleStringWord()
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
		_lexeme.code = keywordIt->second;
	else
		_lexeme.code = LexemeCode::TId;
}

inline void Scanner::HandleDecNum()
{
	NextChar();
	while ('0' <= *curPos && *curPos <= '9')
		if (!NextChar())
			return HandleErrWord();
	if (*curPos == 'l' || *curPos == 'L')
		NextChar();
	if ('a' <= *curPos && *curPos <= 'z' || 'A' <= *curPos && *curPos <= 'Z' || '_' == *curPos)
		return HandleErrWord();

	_lexeme.code = LexemeCode::TDecimNum;
}

inline void Scanner::HandleHexOrOctNum()
{
	NextChar();
	if (*curPos == 'X' || *curPos == 'x')
		return HandleHexNum();
	return HandleOctNum();
}

inline void Scanner::HandleHexNum()
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

	_lexeme.code = LexemeCode::THexNum;
}

inline void Scanner::HandleOctNum()
{
	while ('0' <= *curPos && *curPos <= '7')
		if (!NextChar())
			return HandleErrWord();
	if (*curPos == 'l' || *curPos == 'L')
		NextChar();
	if ('a' <= *curPos && *curPos <= 'z' || 'A' <= *curPos && *curPos <= 'Z' || '8' <= *curPos && *curPos <= '9' || '_' == *curPos)
		return HandleErrWord();

	_lexeme.code = LexemeCode::TOctNum;
}

inline void Scanner::HandleErrWord()
{
	while ('a' <= *curPos && *curPos <= 'z' || 'A' <= *curPos && *curPos <= 'Z'
		|| '0' <= *curPos && *curPos <= '9' || '_' == *curPos)
	{
		NextChar();
	}
	_lexeme.code = LexemeCode::TErr;
}

inline void Scanner::HandleDoubleChar(LexemeCode firstLexeme, char nextChar, LexemeCode secondLexeme)
{
	NextChar();
	if (*curPos == nextChar)
	{
		NextChar();
		_lexeme.code = secondLexeme;
	}
	else
		_lexeme.code = firstLexeme;
}

inline bool Scanner::NextChar()
{
	bool isLexemeOverflow = _lexeme.str.size() > MAX_LEXEME_SIZE;
	if (!isLexemeOverflow)
		_lexeme.str.push_back(*curPos);
	++curPos;
	return !isLexemeOverflow;
}