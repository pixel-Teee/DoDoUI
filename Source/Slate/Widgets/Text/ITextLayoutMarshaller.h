#pragma once

namespace DoDo {
	class DoDoUtf8String;
	class FTextLayout;
	/*
	* interface used to get/set the raw text to/from a text layout
	*/
	class ITextLayoutMarshaller
	{
	public:
		virtual ~ITextLayoutMarshaller() {}

		/*
		* populate the text layout from the string
		*/
		virtual void set_text(const DoDoUtf8String& source_string, FTextLayout& target_text_layout) = 0;

		/*
		* populate the string from the text layout
		*/
		virtual void get_text(const DoDoUtf8String& source_string, FTextLayout& source_text_layout) = 0;

		/*
		* mark this text layout as dirty (eg, because some settings have changed)
		*/
		virtual void make_dirty() = 0;

		virtual void clear_dirty() = 0;

		virtual bool is_dirty() const = 0;
	};
}