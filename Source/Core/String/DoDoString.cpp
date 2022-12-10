#include <PreCompileHeader.h>

#include "DoDoString.h"

namespace DoDo {
	DoDoString::DoDoString()
	{
		m_buffer = nullptr;
	}
	DoDoString::DoDoString(const char* str)
	{
		//don't record \0
		m_buffer = new char[strlen(str)];

		size_t len = strlen(str);
		
		memcpy(m_buffer, str, len);

		m_len = len;
	}
	DoDoString::DoDoString(DoDoString&& rhs)
	{
		//steal resource
		m_buffer = rhs.m_buffer;
		rhs.m_buffer = nullptr;
	}

	DoDoString::~DoDoString()
	{
		if (m_buffer != nullptr)
			delete m_buffer;
	}
}