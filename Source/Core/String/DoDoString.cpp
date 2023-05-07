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

			rhs.m_buffer = nullptr;//note:this is important
			rhs.m_len = 0;
			rhs.m_count = 0;
		}
	}
	DoDoUtf8String::DoDoUtf8String(const DoDoUtf8String& rhs)
	{
		//if (m_buffer != nullptr)
		//{
		//	delete m_buffer;
		//	m_len = 0;
		//	m_count = 0;
		//}

		//copy rhs
		m_buffer = new char [rhs.m_count];
		memcpy(m_buffer, rhs.m_buffer, rhs.m_count);
		m_len = rhs.m_len;
		m_count = rhs.m_count;
	}
	DoDoUtf8String::DoDoUtf8String(std::string u8_str)
	{
		//if (m_buffer != nullptr)
		//{
		//	delete m_buffer;
		//	m_len = 0;
		//	m_count = 0;
		//}

		m_count = strlen(u8_str.c_str()) + 1;

		//copy rhs
		m_buffer = new char[m_count];
		memcpy(m_buffer, u8_str.data(), u8_str.size());
		m_buffer[m_count - 1] = '\0';
		//m_len = u8_str.size();
		//m_count = u8_str.size();
		m_len = calculate_length(m_buffer);
	}
	DoDoUtf8String::DoDoUtf8String(const char* str, int32_t extra_space)
	{
		//str have '\0'
		m_count = strlen(str);//strlen will git rid of str's \0
		m_buffer = new char[m_count + extra_space];

		memset(m_buffer, '\0', m_count + extra_space);//clear

		memcpy(m_buffer, str, m_count);
	}
	DoDoUtf8String::DoDoUtf8String(const DoDoUtf8String& lhs, int32_t extra_space)
	{
		m_buffer = new char[lhs.m_count + extra_space];//git rid of str's \0

		m_count = lhs.m_count;

		memset(m_buffer, '\0', m_count + extra_space);

		memcpy(m_buffer, lhs.m_buffer, m_count);
	}
	DoDoUtf8String::DoDoUtf8String(const DoDoUtf8String& lhs, const DoDoUtf8String& rhs)
	{
		m_buffer = new char[lhs.get_count() + rhs.get_count() - 1];

		memset(m_buffer, '\0', lhs.get_count() + rhs.get_count() - 1);

		m_count = lhs.get_count() + rhs.get_count() - 1;

		m_len = lhs.get_length() + rhs.get_length() - 1;

		memcpy(m_buffer, lhs.m_buffer, lhs.m_count - 1);

		memcpy(m_buffer + lhs.m_count - 1, rhs.m_buffer, rhs.m_count);

		//for (size_t i = lhs.m_count; i <= m_count; ++i)
		//{
		//	m_buffer[i] = rhs.m_buffer[i - lhs.m_count];
		//}
	}
	void DoDoUtf8String::update_length()
	{
		//m_count = sizeof(m_buffer);
		m_len = calculate_length(m_buffer);
	}
	DoDoUtf8String::~DoDoUtf8String()
	{
		if (m_buffer != nullptr)
		{
			delete m_buffer;
			m_len = 0;
			m_count = 0;
			m_buffer = nullptr;
		}
	}
	const char* DoDoUtf8String::c_str() const
	{
		return m_buffer;
	}
	size_t DoDoUtf8String::get_length() const
	{
		return m_len;
	}
	size_t DoDoUtf8String::get_count() const
	{
		return m_count;
	}
	DoDoUtf8String DoDoUtf8String::utf8_at(size_t index)
	{
		//to find index 
		if (!m_need_update_lengths_cache)
		{
			calculate_lengths(m_buffer, m_bytes_counts);
			m_need_update_lengths_cache = true;
		}

		return utf8_sub_str(index, m_bytes_counts[index]);//index character
	}

	DoDoUtf8String DoDoUtf8String::utf8_at(size_t index) const
	{
		//todo:fix me
		//to find index 
		if (!m_need_update_lengths_cache)
		{
			const_cast<DoDoUtf8String*>(this)->calculate_lengths(m_buffer, const_cast<DoDoUtf8String*>(this)->m_bytes_counts);
			const_cast<DoDoUtf8String*>(this)->m_need_update_lengths_cache = true;
		}

		return const_cast<DoDoUtf8String*>(this)->utf8_sub_str(index, m_bytes_counts[index]);//index character
	}

	uint32_t DoDoUtf8String::to_utf16_code_point(size_t index)
	{
		if (!m_need_update_lengths_cache)
		{
			const_cast<DoDoUtf8String*>(this)->calculate_lengths(m_buffer, const_cast<DoDoUtf8String*>(this)->m_bytes_counts);
			const_cast<DoDoUtf8String*>(this)->m_need_update_lengths_cache = true;
		}

		DoDoUtf8String str = utf8_sub_str(index, m_bytes_counts[index]);

		char* code = str.m_buffer;

		//cover to unicode

		uint32_t unicode = 0;

		char c1, c2, c3;
		char tmp;

		if (!(code[0] & 0x80)) //0xxxxxxx
		{
			unicode = code[0] & 0x7f;
		}
		else if ((uint8_t)(code[0]) <= 0xdf && str.m_count - 1 == 2) //110xxxxx
		{
			c1 = (code[0] >> 2) & 0x07;
			c2 = (code[1] & 0x3f) | ((code[0] & 0x03) << 6);

			unicode = ((((uint16_t)(c1) & 0x00ff) << 8) | ((uint16_t)(c2) & 0x00ff));
		}
		else if ((uint8_t)(code[0]) <= 0xef && str.m_count - 1 == 3) //1110xxxx
		{
			c1 = ((uint8_t)code[0] << 4) | ((code[1] >> 2) & 0x0f);
			c2 = (((uint8_t)code[1] << 6) & 0xc0) | (code[2] & 0x3f);
			
			unicode = ((((uint16_t)(c1) & 0x00ff) << 8) | (uint16_t)(c2) & 0x00ff);
		}
		else if ((uint8_t)(code[0]) <= 0xf7 && str.m_count - 1 == 4) //11110xxx
		{
			c1 = (((uint8_t)code[0] << 5) & 0xe0) | ((code[1] >> 4) & 0x03);//3 + 2 = 5 bits
			c2 = (((uint8_t)code[1] << 4) & 0xf0) | ((code[2] >> 2) & 0x0f);//8 bits
			c3 = (((uint8_t)code[2] << 6) & 0xc0) | (code[3] & 0x3f);//8 bits

			uint32_t q1 = (((uint32_t)(c1) & 0x000000ff) << 16);
			uint32_t q2 = ((uint32_t)(c2) & 0x000000ff << 8);
			uint32_t q3 = ((uint32_t)(c3) & 0x000000ff);
			unicode = (((uint32_t)(c1) & 0x000000ff) << 16) | (((uint32_t)(c2) & 0x000000ff) << 8) | ((uint32_t)(c3) & 0x000000ff);
		}

		//covert unicode to utf-16 code

		uint32_t utf16_code;
		if (unicode < 0x10000)
		{
			utf16_code = (uint16_t)unicode;
		}
		else
		{
			uint32_t tmp = unicode - 0x10000;
			uint32_t c1 = (uint32_t)((tmp >> 10) & 0x3ff) + 0xd800;
			uint32_t c2 = (uint32_t)(tmp & 0x3ff) + 0xdc00;
			utf16_code = (c1 << 16) | c2;
		}

		//todo:handle this
		//else
		//{
		//
		//}

		return utf16_code;
	}
	DoDoUtf8String DoDoUtf8String::utf8_sub_str(size_t pos, size_t buffer_count)
	{
		if (!m_need_update_lengths_cache)
		{
			calculate_lengths(m_buffer, m_bytes_counts);
			m_need_update_lengths_cache = true;
		}

		size_t prefix_sum = 0;
		if (pos > 0)
		{
			for (size_t i = 0; i < pos; ++i) //note:prefix sum
			{
				prefix_sum += m_bytes_counts[i];
			}
		}

		std::string str;//todo:optimize me

		for (size_t i = prefix_sum; i < prefix_sum + buffer_count; ++i)
		{
			str.push_back(m_buffer[i]);
		}

		return DoDoUtf8String(str);//note:construct a substr copy
	}
	DoDoUtf8String& DoDoUtf8String::operator=(const DoDoUtf8String& rhs)
	{
		if (m_buffer != nullptr)
		{
			delete m_buffer;
			m_len = 0;
			m_count = 0;
		}

		//copy rhs
		m_buffer = new char[rhs.m_count];
		memcpy(m_buffer, rhs.m_buffer, rhs.m_count);
		m_len = rhs.m_len;
		m_count = rhs.m_count;

		return *this;
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
		
		//don't record '\0' length
		return total_length;
	}

	int32_t DoDoUtf8String::calculate_lengths(char* m_buffer, std::vector<uint32_t>& out_character_byte_counts)
	{
		size_t temp_count = 0;
		size_t total_length = 0;
		uint32_t flag = -1;
		uint32_t len = -1;
		while (m_buffer[temp_count] != '\0')
		{
			uint8_t character = (uint8_t)m_buffer[temp_count];
			if (character <= 0x7f) { //ascii
				++total_length;
				out_character_byte_counts.push_back(1);
			}
			else if (character >= 0xC0 && character <= 0xDF) //occupy 2 bytes
			{
				flag = 1;
				len = 2;
			}
			else if (character >= 0xE0 && character <= 0xEF) //occupy 3 bytes
			{
				flag = 2;
				len = 3;
			}
			else if (character >= 0xF0 && character <= 0xF7) //occupy 4 bytes
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

		//don't record '\0' length
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

	bool operator!=(const DoDoUtf8String& lhs, const DoDoUtf8String& rhs)
	{
		if (lhs.m_len != rhs.m_len || lhs.m_count != rhs.m_count) return true;
		else
		{
			for (size_t i = 0; i < lhs.m_len; ++i)
			{
				if (lhs.m_buffer[i] != rhs.m_buffer[i]) return true;
			}
			return false;
		}
	}

	DoDoUtf8String operator/(const char* lhs, const DoDoUtf8String& rhs)
	{
		//int32_t str_len = rhs.get_length();//have '\0'

		DoDoUtf8String result(lhs, rhs.get_count() + 1);

		result.m_buffer[strlen(lhs) + 1] = '/';

		memcpy(result.m_buffer + strlen(lhs) + 1, rhs.m_buffer, rhs.m_count);

		result.update_length();

		result.m_count = strlen(lhs) + 1 + rhs.get_count();//have '\0'

		return result;
	}

	DoDoUtf8String operator/(const DoDoUtf8String& lhs, const DoDoUtf8String& rhs)
	{
		//return DoDoUtf8String();
		//return DoDoUtf8String(lhs, rhs);

		DoDoUtf8String result(lhs, rhs.m_count + 1);

		result.m_buffer[result.m_count - 1] = '/';

		memcpy(result.m_buffer + result.m_count, rhs.m_buffer, rhs.m_count);

		result.m_count = result.m_count + rhs.m_count;
		//todo:implement path append
		result.update_length();

		return result;
	}

	DoDoUtf8String operator+(const DoDoUtf8String& lhs, const DoDoUtf8String& rhs)
	{
		DoDoUtf8String result;

		if (result.m_buffer != nullptr) delete result.m_buffer;//delete '\0'
		
		result.m_buffer = new char[lhs.m_count - 1 + rhs.m_count];

		memset(result.m_buffer, '\0', lhs.m_count - 1 + rhs.m_count);

		result.m_count = lhs.m_count - 1 + rhs.m_count;

		result.m_len = lhs.m_len - 1 + rhs.m_len;

		memcpy(result.m_buffer, lhs.m_buffer, lhs.m_count - 1);
		memcpy(result.m_buffer + lhs.m_count - 1, rhs.m_buffer, rhs.m_count);

		return result;
	}

	bool DoDoUtf8String::operator<(const DoDoUtf8String& rhs) const //for hash map
	{
		//for (size_t i = 0; i < std::min(m_count, rhs.m_count); ++i)
		//{
		//	if (m_buffer[i] < rhs.m_buffer[i]) return true;
		//}
		//if (m_len != rhs.m_len) return m_len < rhs.m_len;
		//return true;

		//less or equal
		//check ascii
		return std::strcmp(m_buffer, rhs.m_buffer) < 0;//todo:fix me
	}

}