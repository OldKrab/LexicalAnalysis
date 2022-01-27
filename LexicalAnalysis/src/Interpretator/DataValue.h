#pragma once

enum class ValueType
{
	Int, Long
};

struct DataValue
{
	DataValue() = default;
	DataValue(int value) :type(ValueType::Int)
	{
		val.int_ = value;
	}
	DataValue(long long value) :type(ValueType::Long)
	{
		val.long_ = value;
	}
	ValueType type;
	union {
		int int_;
		long long long_;
	} val;
};
