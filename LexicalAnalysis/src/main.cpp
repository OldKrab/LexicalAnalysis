#include <iostream>
#include <fstream>
#include "Scanner.h"


int main()
{
	Scanner scanner("testedSource.cpp");
	std::ofstream fout("output.txt");
	scanner.Scan(fout);
	return 0;
}

