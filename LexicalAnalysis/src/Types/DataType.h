#pragma once
#include <map>
#include <string>

enum class DataType
{
	Int, Short, Long, Bool, Void, Unknown
};


inline std::string DataTypeToString(DataType type) {
	static std::map<DataType, std::string> semStrings = {
		{DataType::Bool, "Bool"},
		{DataType::Unknown, "Unknown"},
		{DataType::Int, "Int"},
		{DataType::Long, "Long"},
		{DataType::Short, "Short"},
		{DataType::Void, "Void"},
	};
	
	return semStrings.at(type);
}

inline DataType LexemeStringToDataType(const std::string& lexStr)
{
	static std::map<std::string, DataType> semStrings = {
		{"bool", DataType::Bool},
		{"int", DataType::Int},
		{"long", DataType::Long},
		{"short", DataType::Short},
		{"void", DataType::Void},
	};
	return semStrings.at(lexStr);
}