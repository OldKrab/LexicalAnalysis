#pragma once
#include "Types/TriadType.h"


struct Triad;

struct Operand
{
	Operand();
	explicit Operand(Triad* triad);
	explicit Operand(std::string view);

	bool IsLink;

	Triad* Triad;

	std::string View;
};

struct Triad
{

	Triad(int id, std::string operation, Operand firstOperand, Operand secondOperand);
	Triad(int id, std::string operation, Operand operand);
	Triad(int id, std::string operation);


	int Id;

	std::string Operation;
	Operand FirstOperand, SecondOperand;
	int OperandsCount;
};

std::ostream& operator<<(std::ostream& out, const Operand& op);
std::ostream& operator<<(std::ostream& out, const Triad& triad);

