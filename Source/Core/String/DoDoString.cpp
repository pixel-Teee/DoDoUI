#include <PreCompileHeader.h>

#include "DoDoString.h"

namespace DoDo {
	DoDoUtf8String::DoDoUtf8String()
	{
		//empty string
		m_len = 0;
		m_count = 1;
		m_buffer = new char[1];
		m_buffer[0] = '\0';//utf8
	}
	DoDoUtf8String::DoDoUtf8String(const char* str)
	{
		if (str == nullptr)
		{
			m_len = 0;
			m_count = 1;
			m_buffer = new char[1];
			m_buffer[0] = '\0';
		}
		else
		{
			//str is not fixed length
			size_t temp_count = 0;
			while(str[temp_count] != '\0')
			{
				++temp_count;
			}

			m_count = temp_count + 1;

			m_buffer = new char[m_count];

			memcpy(m_buffer, str, m_count);

			//calculate m_len

			m_len = calculate_length(m_buffer);
		}
	}
	DoDoUtf8String::DoDoUtf8String(DoDoUtf8String&& rhs)
	{
		//steal resource
		if (rhs.m_buffer != nullptr)
		{
			m_buffer = rhs.m_buffer;
			m_len = rhs.m_len;
			m_count = rhs.m_count;
		}
	}
	DoDoUtf8String::DoDoUtf8String(const DoDoUtf8String& rhs)
	{
		if (m_buffer != nullptr)
		{
			delete m_buffer;
			m_len = 0;
			m_count = 0;
		}

		//copy rhs
		m_buffer = new char [rhs.m_count];
		memcpy(m_buffer, rhs.m_buffer, rhs.m_count);
		m_len = rhs.m_len;
		m_count = rhs.m_count;
	}
	DoDoUtf8String::~DoDoUtf8String()
	{
		if (m_buffer != nullptr)
		{
			delete m_buffer;
			m_len = 0;
			m_count = 0;
		}
	}
	const char* DoDoUtf8String::c_str() const
	{
		return m_buffer;
	}
	size_t DoDoUtf8String::get_length() const
	{
		return m_count;
	}
	size_t DoDoUtf8String::get_count() const
	{
		return m_count;
	}
	int32_t DoDoUtf8String::calculate_length(char* m_buffer)
	{
		size_t temp_count = 0;
		size_t total_length = 0;
		uint32_t flag = -1;
		while (m_buffer[temp_count] != '\0')
		{
			uint8_t character = (uint8_t)m_buffer[temp_count];
			if (character <= 0x7f) { //ascii
				++total_length;
			}
			else if (character >= 0xC0 && character <= 0xDF) //occupy 2 bytes
			{
				flag = 1;
			}
			else if (character >= 0xE0 && character <= 0xEF) //occupy 3 bytes
			{
				flag = 2;
			}
			else if (character >= 0xF0 && character <= 0xF7) //occupy 4 bytes
			{
				flag = 3;
			}
			else if (flag != 0)
			{
				--flag;
				if (flag == 0) ++total_length;
			}
			++temp_count;
		}
		
		//record '\0'
		return total_length + 1;
	}

	int32_t DoDoUtf8String::calculate_lengths(char* m_buffer, std::vector<uint32_t>& out_character_byte_counts)
	{
		size_t temp_count = 0;
		size_t total_length = 0;
		uint32_t flag = -1;
		uint32_t len = -1;
		while (m_buffer[temp_count] != '\0')
		{
			if (m_buffer[temp_count] <= 0x7f) { //ascii
				++total_length;
				out_character_byte_counts.push_back(1);
			}
			else if (m_buffer[temp_count] >= 0xC0 && m_buffer[temp_count] <= 0xDF) //occupy 2 bytes
			{
				flag = 1;
				len = 2;
			}
			else if (m_buffer[temp_count] >= 0xE0 && m_buffer[temp_count] <= 0xEF) //occupy 3 bytes
			{
				flag = 2;
				len = 3;
			}
			else if (m_buffer[temp_count] >= 0xF0 && m_buffer[temp_count] <= 0xF7) //occupy 4 bytes
			{
				flag = 3;
				len = 4;
			}
			else if (flag != 0)
			{
				--flag;
				if (flag == 0)
				{
					++total_length;
					out_character_byte_counts.push_back(len);
				}
			}
			++temp_count;
		}

		//record '\0'
		return total_length + 1;
	}

	bool operator==(const DoDoUtf8String& lhs, const DoDoUtf8String& rhs)
	{
		
		if (lhs.m_count != rhs.m_count) return false;
		else
		{
			for (size_t i = 0; i < lhs.m_count; ++i)
			{
				if (lhs.m_buffer[i] != rhs.m_buffer[i]) return false;
			}
			return true;
		}
	}

}