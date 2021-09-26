#include <iostream>
#include <fstream>
#include "Scanner.h"


int main()
{
	Scanner scanner("testedSource.cpp");
	std::ofstream fout("output.txt");
	std::cout.rdbuf(fout.rdbuf());
	scanner.Scan(std::cout);
	return 0;
}

