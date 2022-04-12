#pragma once
#include <memory>
#include <vector>

#include "Triad.h"

class IntermediateGenerator
{
public:
	Triad* AssignVariable(std::string id, Operand operand);
	void StartFunctionDeclaration(std::string id);
	void EndFunctionDeclaration();
	void StartFor();
	void StartForCond();
	void StartForStep();
	void StartForStat();
	void EndFor();
	Triad* Operation(TriadType operation, const Operand& leftOperand, const Operand& rightOperand);
	Triad* Operation(TriadType operation, const Operand& operand);
	Triad* CallFunction(std::string id, std::vector<Operand> args);


private:
	int GetAndIncTriadId() { return currTriadId++; }

	template<class ...ArgsTypes>
	Triad* AddTriad(ArgsTypes... args);
	int currTriadId = 1;
	std::vector<std::unique_ptr<Triad>> triads;
};

template <class ... ArgsTypes>
Triad* IntermediateGenerator::AddTriad(ArgsTypes... args)
{
	triads.push_back(std::make_unique<Triad>(GetAndIncTriadId(), args...));
	return triads.back().get();
}
