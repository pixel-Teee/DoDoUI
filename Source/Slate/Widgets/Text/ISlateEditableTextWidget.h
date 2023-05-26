#pragma once

#include "SlateCore/Types/SlateEnums.h"//ETextCommit

namespace DoDo
{
	class SWidget;
	class DoDoUtf8String;
	/*interface to allow FSlateEditableTextLayout to notify its parent SEditable/SMultiLineEditableText of changes, as well as query some widget specific state*/
	//note:this will be called from FSlateEditableTextLayout
	class ISlateEditableTextWidget
	{
	public:

		virtual std::shared_ptr<SWidget> get_slate_widget() = 0;

		virtual void on_text_committed(const DoDoUtf8String& in_text, const ETextCommit::Type in_text_action) = 0;
	};
}