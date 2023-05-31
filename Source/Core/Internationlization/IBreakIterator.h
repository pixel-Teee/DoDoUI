#pragma once

#include "Core/String/DoDoString.h"//DoDoUtf8String

namespace DoDo {
	class IBreakIterator
	{
	public:
		virtual ~IBreakIterator() = default;

		//virtual void set_string(const DoDoUtf8String& in_string)
		//{
		//	set_string(in_string);
		//}

		virtual void set_string(DoDoUtf8String in_string) = 0;
	};
}