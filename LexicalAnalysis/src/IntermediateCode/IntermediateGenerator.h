#pragma once
#include <memory>
#include <vector>

#include "Triad.h"
#include "Types/DataType.h"

class IntermediateGenerator
{
public:
	Operand AssignVariable(std::string id, Operand operand);
	void StartFunctionDeclaration(std::string id);
	void EndFunctionDeclaration();
	void StartFor();
	void StartForCond();
	void StartForStep();
	void StartForStat();
	void EndFor();
	Operand Operation(std::string operation, const Operand& leftOperand, const Operand& rightOperand);
	Operand Operation(std::string operation, const Operand& operand);
	Operand CastOperandIfNeed(DataType from, DataType to, Operand operand);
	Operand CallFunction(std::string id, const std::vector<Operand>& args);

	void PrintTriads() const;

private:
	int GetAndIncTriadId() { return currTriadId++; }

	template<class ...ArgsTypes>
	Triad* AddTriad(ArgsTypes... args);
	Triad* AddNop();

	int currTriadId = 1;
	std::vector<std::unique_ptr<Triad>> triads;
};

template <class ... ArgsTypes>
Triad* IntermediateGenerator::AddTriad(ArgsTypes... args)
{
	triads.push_back(std::make_unique<Triad>(GetAndIncTriadId(), std::move(args)...));
	return triads.back().get();
}
