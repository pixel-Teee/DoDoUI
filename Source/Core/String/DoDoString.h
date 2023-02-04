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

		DoDoUtf8String(std::string u8_str);//str is utf8, just to copy

		//this function need to call update_length
		DoDoUtf8String(const char* str, int32_t extra_space);//will add extra_space

		DoDoUtf8String(const DoDoUtf8String& lhs, int32_t extra_space);

		DoDoUtf8String(const DoDoUtf8String& lhs, const DoDoUtf8String& rhs);//combine

		void update_length();

		~DoDoUtf8String();

		size_t get_length() const;

		size_t get_count() const;

		//return c style string
		const char* c_str() const;

		friend bool operator==(const DoDoUtf8String& lhs, const DoDoUtf8String& rhs);

		DoDoUtf8String& operator=(const DoDoUtf8String& rhs);

		//todo:may be to implement this
		//DoDoUtf8String conver_to_DoDoUtf8String(std::wstring str);

		friend DoDoUtf8String operator/(const char* lhs, const DoDoUtf8String& rhs);

		friend DoDoUtf8String operator/(const DoDoUtf8String& lhs, const DoDoUtf8String& rhs);

		bool operator<(const DoDoUtf8String& rhs) const;
	private:
		int32_t calculate_length(char* m_buffer);

		int32_t calculate_lengths(char* m_buffer, std::vector<uint32_t>& out_character_byte_counts);
		//record \0, string is not fixed
		size_t m_len;

		//count numbers bytes
		size_t m_count;

		//as utf8, store \0
		char* m_buffer;
	};
}