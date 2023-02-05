#pragma once

#include <vector>

#include "SlateBrush.h"

namespace DoDo {
	class DoDoUtf8String;
	class FSlateBrush;
	class ISlateStyle
	{
	public:
		/*constructor*/
		ISlateStyle() {}

		/*@return the name used to identity this style set*/
		virtual const DoDoUtf8String& get_style_set_name() const = 0;

		/*populate an array of FSlateBrush with resources consumed by this style
		* @param OutResources - the array to populate
		*/
		virtual void get_resources(std::vector<const FSlateBrush*>& out_resources) const = 0;

		virtual const FSlateBrush* get_brush(const DoDoUtf8String& property_name) const = 0;//todo:implement this add extra parameter

		/*destructor*/
		virtual ~ISlateStyle();
	};
}