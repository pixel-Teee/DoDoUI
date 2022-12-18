#pragma once

#include <iostream>
#include <vector>

namespace DoDo {
	class DoDoUtf8String
	{
	public:
		DoDoUtf8String();

		DoDoUtf8String(const char* str);

		DoDoUtf8String(DoDoUtf8String&& rhs);

		DoDoUtf8String(const DoDoUtf8String& rhs);

		~DoDoUtf8String();

		size_t get_length() const;

		size_t get_count() const;

		//return c style string
		const char* c_str() const;

		friend bool operator==(const DoDoUtf8String& lhs, const DoDoUtf8String& rhs);
	private:
		int32_t calculate_length(char* m_buffer);

		int32_t calculate_lengths(char* m_buffer, std::vector<uint32_t>& out_character_byte_counts);
		//don't record \0, just calculate length of string, string is not fixed
		size_t m_len;

		//count numbers bytes
		size_t m_count;

		//as utf8, store \0
		char* m_buffer;
	};
}