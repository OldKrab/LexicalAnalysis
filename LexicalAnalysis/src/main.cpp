#include <iostream>
#include <fstream>
#include "Syntaxes/SyntaxAnalyser.h"

int main()
{
	setlocale(LC_ALL, "rus");
	std::ofstream fout("output.txt");
	std::ifstream fin("tested.cpp");
	SyntaxAnalyser analyser(fin);
	analyser.PrintAnalysis();
	return 0;
}
