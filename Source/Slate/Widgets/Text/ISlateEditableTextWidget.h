#pragma once

namespace DoDo
{
	class SWidget;
	/*interface to allow FSlateEditableTextLayout to notify its parent SEditable/SMultiLineEditableText of changes, as well as query some widget specific state*/
	//note:this will be called from FSlateEditableTextLayout
	class ISlateEditableTextWidget
	{
	public:

		virtual std::shared_ptr<SWidget> get_slate_widget() = 0;
	};
}