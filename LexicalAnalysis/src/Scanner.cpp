#include <fstream>
#include <sstream>

#include "Scanner.h"

 std::unordered_map<std::string, LexemeCode> Scanner::keywords = {
 	{"for", LexemeCode::TFor},
 	{"int", LexemeCode::TInt},
 	{"short", LexemeCode::TShort},
 	{"long", LexemeCode::TLong},
 	{"void", LexemeCode::TVoid},
 	{"main", LexemeCode::TMain},
 	{"bool", LexemeCode::TBool}
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
