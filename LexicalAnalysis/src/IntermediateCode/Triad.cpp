#include "Triad.h"

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

Triad::Triad(int id, TriadType operation, Operand firstOperand, Operand secondOperand)
	: Id(id),
	Operation(operation),
	FirstOperand(std::move(firstOperand)),
	SecondOperand(std::move(secondOperand)),
	OperandsCount(2)
{
}

Triad::Triad(int id, TriadType operation, Operand operand)
	: Id(id),
	Operation(operation),
	FirstOperand(std::move(operand)),
	OperandsCount(1)
{
}
