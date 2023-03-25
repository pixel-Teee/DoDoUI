#pragma once

#include "SlateCore/Types/SlateEnums.h"//EHorizontalAlignment depends on it

namespace DoDo {
	/*
	* interface for window title bars
	*/
	class IWindowTitleBar
	{
	public:
	};

	class SWindow;
	class SWidget;
	struct FWindowTitleBarArgs
	{
		FWindowTitleBarArgs(const std::shared_ptr<SWindow>& in_window)
			: m_window(in_window)
			, m_center_content_alignment(HAlign_Fill)
		{

		}

		/*the window to create the title bar for*/
		std::shared_ptr<SWindow> m_window;

		/*optional content for the title bar's center (will override window title)*/
		std::shared_ptr<SWidget> m_center_content;

		/*the horizontal alignment of the center content*/
		EHorizontalAlignment m_center_content_alignment;
	};
}