#include "IntermediateGenerator.h"

Triad* IntermediateGenerator::AssignVariable(std::string id, Operand operand)
{
	return AddTriad(TriadType::Assign, Operand(id), operand);
}

Triad* IntermediateGenerator::Operation(TriadType operation, const Operand& leftOperand, const Operand& rightOperand)
{
	return AddTriad(operation, leftOperand, rightOperand);
}

Triad* IntermediateGenerator::Operation(TriadType operation, const Operand& operand)
{
	return AddTriad(operation, operand);
}

Triad* IntermediateGenerator::CallFunction(std::string id, std::vector<Operand> args)
{
	for(auto&& arg: args)
	{
		AddTriad();
	}

	
}

