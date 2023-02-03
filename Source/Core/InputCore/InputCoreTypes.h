#pragma once

#include "Core/String/DoDoString.h"

namespace DoDo
{
	struct FKey
	{
		FKey()
		{
			
		}

		FKey(const DoDoUtf8String& in_name)
			: m_key_name(in_name)
		{}

	private:
		DoDoUtf8String m_key_name;

		//todo:implement FKeyDatils
	};
}