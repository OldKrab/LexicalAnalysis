#pragma once
#include "Types/DataType.h"
#include <iostream>

struct DataValue
{
	DataValue() = default;
	DataValue(int value) :type(DataType::Int), intVal(value)
	{}

	DataValue(long long value) :type(DataType::Long), longVal(value)
	{}

	explicit DataValue(DataType type) :type(type), longVal(0)
	{}

	DataType type = DataType::Int;
	union {
		int intVal;
		long long longVal;
	};
};

inline std::ostream& operator<<(std::ostream& out, const DataValue& value)
{
	return out << "(" << DataTypeToString(value.type) << ")" << (value.type == DataType::Long ? value.longVal : value.intVal);
}
