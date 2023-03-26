#include <PreCompileHeader.h>

#include "Parse.h"

#include <string>

namespace DoDo {
	uint32_t FParse::hex_number(const char* hex_string)
	{
		uint32_t ret = 0;

		while (*hex_string)
		{
			ret *= 16;
			ret += FParse::hex_digit(*hex_string++);
		}

		return ret;
	}
	bool FParse::Value(const char* stream, const char* match, float& Value)
	{
		std::string new_stream = stream;
		std::string new_match = match;

		if (new_stream.find(new_match) == std::string::npos)
		{
			return false;
		}

		size_t match_pos = new_stream.find(new_match);

		//todo:use microsoft atof
		std::string new_value;
		//stream + strlen(match);

		for (const char* start = stream + match_pos + strlen(match); *start != ',' && *start != '\0'; ++start)
		{
			new_value.push_back(*start);
		}

		Value = std::stof(new_value);

		return true;
	}
}