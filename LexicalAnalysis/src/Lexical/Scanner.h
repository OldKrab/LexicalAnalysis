#pragma once
#include <iomanip>
#include <unordered_map>
#include <string>
#include "Lexeme.h"
#include "SourceText.h"


class Scanner
{
public:
	Scanner(const std::string& sourceFile);
	void Scan(std::ostream& out);
	Lexeme NextScan();
	Lexeme LookForward(int k);
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
	void InputSourceText(const std::string& sourceFile);

	SourceText sourceText;
	SourceText::Iterator curPos;
	Lexeme _lexeme;

	static std::unordered_map<std::string, LexemeType> keywords;
	static const int MAX_LEXEME_SIZE = 100;
};
