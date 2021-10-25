#include <iostream>
#include <fstream>
#include "Scanner.h"
#include "SyntaxAnalyser.h"

int main()
{
	setlocale(LC_ALL, "rus");
	Scanner scanner("tested.cpp");
	std::ofstream fout("output.txt");
	SyntaxAnalyser analyser(&scanner);
	try{
	analyser.TProgram();
	}catch (std::exception& e)
	{
		std::cout << e.what();
		return 1;
	}
	std::cout << "Analysis success";
	return 0;
}

