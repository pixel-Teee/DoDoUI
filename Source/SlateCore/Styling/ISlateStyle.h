#pragma once

namespace DoDo {
	class DoDoUtf8String;
	class ISlateStyle
	{
	public:
		/*constructor*/
		ISlateStyle() {}

		/*@return the name used to identity this style set*/
		virtual const DoDoUtf8String& get_style_set_name() const = 0;

		/*destructor*/
		virtual ~ISlateStyle();
	};
}