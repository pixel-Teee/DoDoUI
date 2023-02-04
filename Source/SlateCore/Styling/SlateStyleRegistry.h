#pragma once

#include <map>

#include "Core/String/DoDoString.h"

namespace DoDo {
	/*
	* a central repository that can be used to track an manage chunks of slate style data
	*/
	class ISlateStyle;
	class FSlateStyleRegistry
	{
	public:
		/*
		* add a slate style to the reposity
		* 
		* @param ISlateStyle interface to a user-definable style to add
		*/
		static void register_slate_style(const ISlateStyle& in_slate_style);

	private:
		/*repository is just a collection of shared style pointers*/
		static std::map<DoDoUtf8String, const ISlateStyle*> m_slate_style_repositry;
	};
}