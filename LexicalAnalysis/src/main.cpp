#include <iostream>
#include <fstream>
#include "Syntaxes/SyntaxAnalyser.h"

int main()
{
	setlocale(LC_ALL, "rus");
	std::ofstream fout("output.txt");
	SyntaxAnalyser analyser("tested.cpp");
	analyser.PrintAnalysis();
	return 0;
}
