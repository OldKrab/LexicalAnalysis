#pragma once
#include <iomanip>
#include <unordered_map>
#include <map>
#include <string>
#include "LexemeCode.h"


class Scanner
{
public:
	Scanner(const std::string& sourceFile);
	inline void Scan(std::ostream& out);
private:
	inline LexemeCode NextScan(std::string& lexemeStr);

	inline void SkipIgnoreChars();
	inline void SkipComment();
	inline LexemeCode HandleStringWord(std::string& lexemeStr);
	inline LexemeCode HandleDecNum(std::string& lexemeStr);
	inline LexemeCode HandleHexOrOctNum(std::string& lexemeStr);
	inline LexemeCode HandleHexNum(std::string& lexemeStr);
	inline LexemeCode HandleOctNum(std::string& lexemeStr);

	inline LexemeCode HandleErrNum(std::string& lexemeStr);

	inline LexemeCode HandleDoubleChar(std::string& lexemeStr, LexemeCode firstLexeme, char nextChar, LexemeCode secondLexeme);

	inline void NextChar(std::string& lexemeStr);
	void InputSourceText(const std::string& sourceFile);

	std::string sourceText;
	std::string::iterator curPos;
	static std::unordered_map<std::string, LexemeCode> keywords;
	static const int MAX_LEXEME_SIZE = 200;
};


inline void Scanner::Scan(std::ostream& out)
{
	std::string lexemeStr;
	LexemeCode lexemeCode = LexemeCode::TAssign;
	while (lexemeCode != LexemeCode::TEnd) {
		lexemeCode = NextScan(lexemeStr);
		out.width(9);
		out.flags(out.left);
		out << lexemeStr << " - " << LexicalCodeToString(lexemeCode) << std::endl;
	}

}


inline LexemeCode Scanner::NextScan(std::string& lexemeStr)
{
	lexemeStr.clear();
	lexemeStr.reserve(MAX_LEXEME_SIZE);

	SkipIgnoreChars();

	if (*curPos == 0)
		return LexemeCode::TEnd;

	if ('a' <= *curPos && *curPos <= 'z' ||
		'A' <= *curPos && *curPos <= 'Z' || '_' == *curPos)
		return HandleStringWord(lexemeStr);

	if ('1' <= *curPos && *curPos <= '9')
		return HandleDecNum(lexemeStr);

	switch (*curPos)
	{
	case '0':
		return HandleHexOrOctNum(lexemeStr);
	case ',':
		lexemeStr.push_back(*curPos++);
		return LexemeCode::TComma;
	case ';':
		lexemeStr.push_back(*curPos++);
		return LexemeCode::TSemi;
	case '(':
		lexemeStr.push_back(*curPos++);
		return LexemeCode::TOpenPar;
	case ')':
		lexemeStr.push_back(*curPos++);
		return LexemeCode::TClosePar;
	case '{':
		lexemeStr.push_back(*curPos++);
		return LexemeCode::TOpenBrace;
	case '}':
		lexemeStr.push_back(*curPos++);
		return LexemeCode::TCloseBrace;
	case '*':
		lexemeStr.push_back(*curPos++);
		return LexemeCode::TMul;
	case '/':
		lexemeStr.push_back(*curPos++);
		return LexemeCode::TDiv;
	case '%':
		lexemeStr.push_back(*curPos++);
		return LexemeCode::TModul;
	case '+':
		return HandleDoubleChar(lexemeStr, LexemeCode::TPlus, '+', LexemeCode::TInc);
	case '-':
		return HandleDoubleChar(lexemeStr, LexemeCode::TMinus, '-', LexemeCode::TDec);
	case '>':
		return HandleDoubleChar(lexemeStr, LexemeCode::TG, '=', LexemeCode::TGE);
	case '<':
		return HandleDoubleChar(lexemeStr, LexemeCode::TL, '=', LexemeCode::TLE);
	case '=':
		return HandleDoubleChar(lexemeStr, LexemeCode::TAssign, '=', LexemeCode::TE);
	case '!':
		return HandleDoubleChar(lexemeStr, LexemeCode::TErr, '=', LexemeCode::TNE);
	default:
		lexemeStr.push_back(*curPos++);
		return LexemeCode::TErr;
	}
}


inline void Scanner::SkipIgnoreChars()
{
	while (curPos != sourceText.end())
	{
		switch (*curPos)
		{
		case '/':
			if (*(curPos + 1) != '/')
				return;
			SkipComment();
			break;
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
	curPos += 2;
	while (*curPos != 0 && *curPos != '\n')
		++curPos;
}

inline LexemeCode Scanner::HandleStringWord(std::string& lexemeStr)
{
	NextChar(lexemeStr);
	while ('a' <= *curPos && *curPos <= 'z'
		|| 'A' <= *curPos && *curPos <= 'Z'
		|| '0' <= *curPos && *curPos <= '9'
		|| '_' == *curPos)
	{
		NextChar(lexemeStr);
	}
	auto keywordIt = keywords.find(lexemeStr);
	if (keywordIt != keywords.end())
		return keywordIt->second;
	return LexemeCode::TId;
}

inline LexemeCode Scanner::HandleDecNum(std::string& lexemeStr)
{
	NextChar(lexemeStr);
	while ('0' <= *curPos && *curPos <= '9')
		NextChar(lexemeStr);

	if ('a' <= *curPos && *curPos <= 'z' || 'A' <= *curPos && *curPos <= 'Z' || '_' == *curPos)
		return HandleErrNum(lexemeStr);

	return LexemeCode::TDecimNum;
}

inline LexemeCode Scanner::HandleHexOrOctNum(std::string& lexemeStr)
{
	NextChar(lexemeStr);
	if (*curPos == 'X' || *curPos == 'x')
		return HandleHexNum(lexemeStr);
	return HandleOctNum(lexemeStr);
}

inline LexemeCode Scanner::HandleHexNum(std::string& lexemeStr)
{
	NextChar(lexemeStr);
	if (!('0' <= *curPos && *curPos <= '9' || 'a' <= *curPos && *curPos <= 'f' || 'A' <= *curPos && *curPos <= 'F'))
		return HandleErrNum(lexemeStr);
	while ('0' <= *curPos && *curPos <= '9' || 'a' <= *curPos && *curPos <= 'f' || 'A' <= *curPos && *curPos <= 'F')
		NextChar(lexemeStr);
	if ('f' < *curPos && *curPos <= 'z' || 'F' < *curPos && *curPos <= 'Z' || '_' == *curPos)
		return HandleErrNum(lexemeStr);

	return LexemeCode::THexNum;
}

inline LexemeCode Scanner::HandleOctNum(std::string& lexemeStr)
{
	while ('0' <= *curPos && *curPos <= '7')
		NextChar(lexemeStr);
	if ('a' <= *curPos && *curPos <= 'z' || 'A' <= *curPos && *curPos <= 'Z' || '8' <= *curPos && *curPos <= '9' || '_' == *curPos)
		return HandleErrNum(lexemeStr);

	return LexemeCode::TOctNum;
}

inline LexemeCode Scanner::HandleErrNum(std::string& lexemeStr)
{
	while ('a' <= *curPos && *curPos <= 'z' || 'A' <= *curPos && *curPos <= 'Z'
		|| '0' <= *curPos && *curPos <= '9' || '_' == *curPos)
	{
		NextChar(lexemeStr);
	}
	return LexemeCode::TErr;
}

inline LexemeCode Scanner::HandleDoubleChar(std::string& lexemeStr, LexemeCode firstLexeme, char nextChar, LexemeCode secondLexeme)
{
	NextChar(lexemeStr);
	if (*curPos == nextChar)
	{
		NextChar(lexemeStr);
		return secondLexeme;
	}
	return firstLexeme;
}

inline void Scanner::NextChar(std::string& lexemeStr)
{
	if (lexemeStr.size() < MAX_LEXEME_SIZE)
		lexemeStr.push_back(*curPos);
	++curPos;
}