#pragma once
#include "Types/DataType.h"


struct DataValue
{
	DataValue() = default;
	DataValue(int value) :type(DataType::Int), intVal(value)
	{}

	DataValue(long long value) :type(DataType::Long), longVal(value)
	{}

	explicit DataValue(DataType type) :type(type), longVal(0)
	{}

	DataType type;
	union {
		int intVal;
		long long longVal;
	};
};
