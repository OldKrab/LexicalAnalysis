#pragma once
#include <iomanip>
#include <unordered_map>
#include <string>
#include "Lexeme.h"
#include "SourceText.h"


class Scanner
{
public:
	explicit Scanner(const std::istream& sourceStream);
	void Scan(std::ostream& out);
	Lexeme NextScan();
	Lexeme LookForward(int k);
	SourceText::Iterator GetCurPos() { return curPos; }
	void SetCurPos(SourceText::Iterator pos) { curPos = pos; }
private:
	void SkipIgnoreChars();
	void SkipComment();

	void HandleStringWord();
	void HandleDecNum();
	void HandleHexOrOctNum();
	void HandleHexNum();
	void HandleOctNum();
	void HandleErrWord();
	void HandleDoubleChar(LexemeType firstLexeme, char nextChar, LexemeType secondLexeme);

	bool NextChar();
	void InputSourceText(const std::istream& sourceStream);

	SourceText sourceText;
	SourceText::Iterator curPos;
	Lexeme _lexeme;

	static std::unordered_map<std::string, LexemeType> keywords;
	static const int MAX_LEXEME_SIZE = 100;
};
