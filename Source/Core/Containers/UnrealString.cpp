#include "PreCompileHeader.h"

#include "UnrealString.h"

#include "Core/String/DoDoString.h"

namespace DoDo {
	void FTextRange::calculate_line_ranges_from_string(const DoDoUtf8String& input, std::vector<FTextRange>& line_ranges)
	{
		int32_t line_begin_index = 0;

		//loop through splitting at new-lines
		
		for (size_t i = 0; i < input.get_length(); ++i)
		{
			//handle a chain of \r\n slightly differently to stop the FChar::IsLineBreak adding two separate new-lines
			const bool b_is_windows_new_line = input.utf8_code_at(i) == u8'\r' && input.utf8_code_at(i + 1) == u8'\n';

			if (b_is_windows_new_line) //todo:check is line break
			{
				const int32_t line_end_index = i;

				line_ranges.emplace_back(FTextRange(line_begin_index, line_end_index));

				if (b_is_windows_new_line)
				{
					++i;//skip the \n of the \r\n chain
				}
				line_begin_index = i + 1;//the next line begins after the end of the current line
			}
		}

		//todo:process any remaining string after the last new-line
		if (line_begin_index <= input.get_length())
		{
			line_ranges.emplace_back(FTextRange(line_begin_index, input.get_length()));
		}
	}
}


