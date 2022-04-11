#include "VarData.h"

#include <cassert>


void VarData::Print(std::ostream& out) const
{
	out << "Variable Node: Type = " << DataTypeToString(Type) << ", Id = " << Identifier << ", Is Initialized = " << IsInitialized << "\n";
}

std::unique_ptr<NodeData> VarData::Clone() const
{
	auto data = std::make_unique<VarData>(Identifier, Type);
	data->IsInitialized = IsInitialized;
	return std::move(data);
}


