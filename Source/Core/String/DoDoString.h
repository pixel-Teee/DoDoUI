#pragma once

namespace DoDo {
	
	class DoDoString
	{
	public:
		//DoDoString("")
		DoDoString();

		DoDoString(const char* str);

		DoDoString(DoDoString&& str);//move constructor

		~DoDoString();
	private:
		uint32_t m_len;
		//as utf-8 store, don't record '\0'
		char* m_buffer;
	};
}