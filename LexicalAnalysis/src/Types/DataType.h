#pragma once
#include <map>
#include <string>

enum class DataType
{
	Int,  Long, Void, Unknown
};


inline std::string DataTypeToString(DataType type) {
	static std::map<DataType, std::string> semStrings = {
		{DataType::Unknown, "Unknown"},
		{DataType::Int, "Int"},
		{DataType::Long, "Long"},
		{DataType::Void, "Void"},
	};
	
	return semStrings.at(type);
}

inline DataType LexemeStringToDataType(const std::string& lexStr)
{
	static std::map<std::string, DataType> semStrings = {
		{"int", DataType::Int},
		{"long", DataType::Long},
		{"void", DataType::Void},
	};
	return semStrings.at(lexStr);
}