#pragma once

#include "SCompoundWidget.h"

namespace DoDo
{
	class SWindow : public SCompoundWidget
	{
	public:

		//SWindow();

		//todo:implement FSlateWindowElementList FWidgetStyle 
		//int32_t paint_window(double current_time, float delta_time);

		virtual ~SWindow() {}

		/*return true if the window is visible, false otherwise*/
		bool is_visible() const;

	protected:
		/*the native window that is backing this slate window*/
		//todo:implement FGenericWindow
	};
}