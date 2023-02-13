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

	struct EKeys
	{
		static const FKey Invalid;

		static const FKey LeftMouseButton;
		static const FKey RightMouseButton;
		static const FKey MiddleMouseButton;
		static const FKey ThumbMouseButton;
		static const FKey ThumbMouseButton2;
	};
}