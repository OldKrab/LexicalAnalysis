#pragma once
#include "Types/DataType.h"
#include "Types/LexemeType.h"
#include "Types/TriadType.h"


struct Triad;

struct Operand
{
	Operand();
	explicit Operand(Triad* triad);
	Operand(std::string view);

	bool IsLink;

	Triad* Triad;

	std::string View;
};

struct Triad
{

	Triad(int id, TriadType operation, Operand firstOperand, Operand secondOperand);
	Triad(int id, TriadType operation, Operand operand);

	int Id;

	TriadType Operation;
	Operand FirstOperand, SecondOperand;
	int OperandsCount;
};

