#pragma once

#include <map>

#include "Core/String/DoDoString.h"

namespace DoDo {
	/*
	* a central repository that can be used to track an manage chunks of slate style data
	*/
	class ISlateStyle;
	struct FSlateBrush;
	class FSlateStyleRegistry
	{
	public:
		/*
		* add a slate style to the reposity
		* 
		* @param ISlateStyle interface to a user-definable style to add
		*/
		static void register_slate_style(const ISlateStyle& in_slate_style);


		/*
		* populate an array of slate brushes with all of the resources used by the registered styles
		* 
		* @param OutResources array of slate brushes to populate
		*/
		static void get_all_resources(std::vector<const FSlateBrush*>& out_resources);

		static const ISlateStyle* find_slate_style(const DoDoUtf8String& in_slate_style_name);

	private:
		/*repository is just a collection of shared style pointers*/
		static std::map<DoDoUtf8String, const ISlateStyle*> m_slate_style_repositry;//don't hold the ownership of ISlateStyle
	};
}