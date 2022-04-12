#include "IntermediateGenerator.h"

#include <iostream>
#include <utility>

Operand IntermediateGenerator::AssignVariable(std::string id, Operand operand)
{
	return Operand(AddTriad("=", Operand(std::move(id)), std::move(operand)));
}

void IntermediateGenerator::StartFunctionDeclaration(std::string id)
{
	AddTriad("proc", Operand(std::move(id)));
	AddTriad("prolog");
}

void IntermediateGenerator::EndFunctionDeclaration()
{
	AddTriad("epilog");
	AddTriad("ret");
	AddTriad("endp");
}

Operand IntermediateGenerator::Operation(std::string operation, const Operand& leftOperand, const Operand& rightOperand)
{
	return Operand(AddTriad(std::move(operation), leftOperand, rightOperand));
}

Operand IntermediateGenerator::Operation(std::string operation, const Operand& operand)
{
	return Operand(AddTriad(std::move(operation), operand));
}

Operand IntermediateGenerator::CastOperandIfNeed(DataType from, DataType to, Operand operand)
{
	if (from != to)
	{
		std::string operation;
		if (from == DataType::Int && to == DataType::Long)
			operation = "I->L";
		else
			operation = "L->I";
		operand = Operand(AddTriad(operation, operand));
	}
	return operand;
}

Operand IntermediateGenerator::CallFunction(std::string id, const std::vector<Operand>& args)
{
	for (auto&& arg : args)
		AddTriad("push", arg);
	return Operand(AddTriad("call", Operand(std::move(id))));
}

void IntermediateGenerator::PrintTriads() const
{
	for (auto&& triad : triads)
		std::cout << *triad << std::endl;
}

Triad* IntermediateGenerator::AddNop()
{
	return AddTriad("nop");
}



