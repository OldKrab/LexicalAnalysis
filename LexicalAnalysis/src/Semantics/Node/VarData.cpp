#include "VarData.h"

#include <cassert>


void VarData::Print(std::ostream& out) const
{
	out << "Variable Node: Type = " << DataTypeToString(Type) << ", Id = " << Identifier << ", Value = ";
	if (Value->type == DataType::Int)
		out << Value->intVal;
	else if (Value->type == DataType::Long)
		out << Value->longVal;
	out << ", Is Initialized = " << IsInitialized << "\n";
}

std::unique_ptr<NodeData> VarData::Clone() const
{
	auto data = std::make_unique<VarData>(Identifier, Type);
	data->IsInitialized = IsInitialized;
	data->Value = std::make_shared<DataValue>(*Value);
	return std::move(data);
}

void VarData::SetDefaultValue(DataType type)
{
	assert(type == DataType::Long || type == DataType::Int);
	switch (type)  
	{
	case DataType::Long:
		Value = std::make_shared<DataValue>(0LL);
		break;
	case DataType::Int:
		Value = std::make_shared<DataValue>(0);
		break;
	default: break;
	}
}
