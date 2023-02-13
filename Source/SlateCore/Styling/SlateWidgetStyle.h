#pragma once

#include <vector>

#include "Core/String/DoDoString.h"

namespace DoDo
{
	struct FSlateBrush;

	/*
	 * base structure for widget style
	 */
	struct FSlateWidgetStyle
	{
	public:
		/*default constructor*/
		FSlateWidgetStyle();

		/*virtual destructor*/
		virtual ~FSlateWidgetStyle(){ }

	public:
		/*
		 * gets the brush resources associated with this style
		 *
		 * this method must be implemented by inherited structures
		 *
		 * @param OutBrushes the brush resources
		 */
		virtual void get_resources(std::vector<const FSlateBrush*>& out_brushes) const {}

		/*
		 * gets the name of this style
		 *
		 * this method must be implemented by inherited structures
		 *
		 * @return widget style name
		 */
		virtual const DoDoUtf8String get_type_name() const
		{
			return DoDoUtf8String("None");
		}
	};
}