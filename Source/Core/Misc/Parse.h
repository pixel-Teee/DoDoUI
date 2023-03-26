#pragma once

#include "Core/Core.h"

namespace DoDo {
	struct FParse
	{
		//parse a hex digit
		static int32_t hex_digit(char c)
		{
			int32_t result = 0;

			if (c >= '0' && c <= '9')
			{
				result = c - '0';
			}
			else if (c >= 'a' && c <= 'f')
			{
				result = c + 10 - 'a';
			}
			else if (c >= 'A' && c <= 'F')
			{
				result = c + 10 - 'A';
			}
			else
			{
				result = 0;
			}

			return result;
		}
		/*parse a hexadecimal string value*/
		static uint32_t hex_number(const char* hex_string);

		/*parses a floating-point value*/
		static bool Value(const char* stream, const char* match, float& Value);
	};
}