#pragma once
#include <string>
#include <utility>

#include "Types/DataType.h"
#include "Types/SemanticType.h"

class NodeData
{
public:
	explicit NodeData(std::string identifier)
		: Identifier(std::move(identifier)) {	}

	virtual DataType GetDataType() const = 0;

	virtual SemanticType GetSemanticType() const = 0;

	virtual std::unique_ptr<NodeData> Clone() const = 0;

	virtual void Print(std::ostream& out) const = 0;

	std::string Identifier;

	virtual ~NodeData() = default;
};
