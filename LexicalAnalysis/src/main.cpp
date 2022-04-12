#include <iostream>
#include <fstream>
#include "Syntaxes/SyntaxAnalyser.h"

int main()
{
	unsigned char a = 1;
	a = a * 2;
	unsigned long long b = a;
	setlocale(LC_ALL, "rus");
	std::ofstream fout("output.txt");
	std::ifstream fin("tested.cpp");
	SyntaxAnalyser analyser(fin);
	analyser.Program();
	return 0;
}
