#pragma once
#include "Node.h"
#include <iostream>
#include <utility>

#include "DataValue.h"
#include "Types/DataType.h"

class VarData : public NodeData
{
public:

	VarData(std::string id, DataType type)
		: NodeData(std::move(id)),
		Type(type),
		IsInitialized(false)
	{
		SetDefaultValue(type);
	}

	DataType GetDataType() const override { return Type; }

	SemanticType GetSemanticType() const override { return SemanticType::Var; }

	void Print(std::ostream& out) const override;

	std::unique_ptr<NodeData> Clone() const override;

	void SetDefaultValue(DataType type);

	DataType Type;
	std::shared_ptr<DataValue> Value;
	bool IsInitialized;
};
