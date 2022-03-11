#pragma once
#include "Node.h"
#include <iostream>

#include "Lexical/SourceText.h"

class FuncData :public NodeData
{
public:
	FuncData(std::string id) :NodeData(std::move(id)) {}

	DataType GetDataType() const override { return DataType::Void; }

	SemanticType GetSemanticType() const override { return SemanticType::Func; }

	void Print(std::ostream& out) const override;

	std::unique_ptr<NodeData> Clone() const override;

	int ParamsCount = 0;
	SourceText::Iterator Pos;
};
