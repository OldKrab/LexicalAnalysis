#include <iostream>
#include <fstream>
#include "Syntaxes/SyntaxAnalyser.h"

int main()
{
	setlocale(LC_ALL, "rus");
	std::ofstream fout("output.txt");
	SyntaxAnalyser analyser("tested.cpp");
	try {
		analyser.StartAnalysis();
	}
	catch (std::exception& e)
	{
		std::cout << e.what();
		return 1;
	}
	std::cout << "Analysis success";
	return 0;
}
