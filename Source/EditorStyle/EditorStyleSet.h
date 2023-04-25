#pragma once

#include "SlateCore/Styling/AppStyle.h"

namespace DoDo {
	/*
	* a collection of named properties that guide the appearance of slate
	*/
	class FEditorStyle
	{
	public:
		/*
		* @return the application style
		* 
		* note: until the editor can be fully updated, calling FEditorStyle::Get() or any of its
		* static convenience functions will return the app style instead of the style defined in this class
		* 
		* using the app style is preferred in most cases as it allows the style to be changed
		* on an application level
		* 
		* in cases requiring explicit use of the EditorStyle where a slate widget should not take on
		* the appearance of the rest of the application, use FEditorStyle::get editor style()
		*/
		static const ISlateStyle& get()
		{
			return FAppStyle::get();
		}

		static void reset_to_default();

	protected:
		static void set_style(const std::shared_ptr<ISlateStyle>& new_style);

	private:
		/*singleton instance of the slate style*/
		static std::shared_ptr<ISlateStyle> m_instance;
	};
}