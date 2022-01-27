#include "FuncData.h"

void FuncData::Print(std::ostream& out) const
{
	out << "Function Node: Id = " << Identifier << ", Param Count = " << ParamsCount << "\n";
}

std::unique_ptr<NodeData> FuncData::Clone() const
{
	auto data = std::make_unique<FuncData>(Identifier);
	data->ParamsCount = ParamsCount;
	return std::move(data);
}
