#pragma once

#include "BaseTextLayoutMarshaller.h"

#include "Core/Core.h"

namespace DoDo {
	/*
	* get/set the raw text to/from a text layout as plain text
	*/
	class FPlainTextLayoutMarshaller : public FBaseTextLayoutMarshaller
	{
	public: 
		static std::shared_ptr<FPlainTextLayoutMarshaller> Create();

		virtual ~FPlainTextLayoutMarshaller();

		/*
		* populate the text layout from the string
		*/
		virtual void set_text(const DoDoUtf8String& source_string, FTextLayout& target_text_layout) override;

		/*
		* populate the string from the text layout
		*/
		virtual void get_text(const DoDoUtf8String& source_string, FTextLayout& source_text_layout) override;
	};
}