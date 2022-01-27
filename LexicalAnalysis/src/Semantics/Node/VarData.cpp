#include "VarData.h"

#include "Syntaxes/SyntaxAnalysisException.h"

void VarData::Print(std::ostream& out) const
{
	out << "Variable Node: Type = " << DataTypeToString(Type) << ", Id = " << Identifier << ", Is Initialized = " << IsInitialized << "\n";
}

std::unique_ptr<NodeData> VarData::Clone() const
{
	auto data = std::make_unique<VarData>(Identifier, Type);
	data->IsInitialized = IsInitialized;
	data->Value = Value;
	return std::move(data);
}

void VarData::SetDefaultValue(DataType type)
{
	switch (type)
	{
	case DataType::Long:
		Value = DataValue(0LL);
		break;
	case DataType::Int:
		Value = DataValue(0);
		break;
	default:
		throw std::exception("Интерпретируются только int и long!");
	}
}
