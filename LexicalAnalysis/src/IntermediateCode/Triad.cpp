#include "Triad.h"

#include <iostream>
#include <string>
#include <utility>


Operand::Operand()
	: IsLink(false),
	Triad(nullptr)
{
}

Operand::Operand(::Triad* triad)
	: IsLink(true),
	Triad(triad)
{
}

Operand::Operand(std::string view)
	: IsLink(false),
	Triad(nullptr),
	View(std::move(view))
{
}

Triad::Triad(int id, std::string operation, Operand firstOperand, Operand secondOperand)
	: Id(id),
	Operation(std::move(operation)),
	FirstOperand(std::move(firstOperand)),
	SecondOperand(std::move(secondOperand)),
	OperandsCount(2)
{
}

Triad::Triad(int id, std::string operation, Operand operand)
	: Id(id),
	Operation(std::move(operation)),
	FirstOperand(std::move(operand)),
	OperandsCount(1)
{
}

Triad::Triad(int id, std::string operation)
	: Id(id),
	Operation(std::move(operation)),
	OperandsCount(0)
{
}

std::ostream& operator<<(std::ostream& out, const Operand& op)
{
	if (op.IsLink)
		out << "(" << op.Triad->Id << ")";
	else
		out << op.View;
	return out;
}

std::ostream& operator<<(std::ostream& out, const Triad& triad)
{
	std::cout << triad.Id << ") " << triad.Operation;
	if (triad.OperandsCount > 0) out << " " << triad.FirstOperand;
	if (triad.OperandsCount > 1) out << " " << triad.SecondOperand;
	return out;
}


