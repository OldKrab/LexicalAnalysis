#include <fstream>
#include <sstream>

#include "Scanner.h"

 std::unordered_map<std::string, LexemeType> Scanner::keywords = {
 	{"for", LexemeType::TFor},
 	{"int", LexemeType::TInt},
 	{"short", LexemeType::TShort},
 	{"long", LexemeType::TLong},
 	{"void", LexemeType::TVoid},
 	{"main", LexemeType::TMain},
 	{"bool", LexemeType::TBool}
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
