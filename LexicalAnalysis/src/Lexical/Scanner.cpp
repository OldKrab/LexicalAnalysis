#include <fstream>
#include <sstream>

#include "Scanner.h"

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
