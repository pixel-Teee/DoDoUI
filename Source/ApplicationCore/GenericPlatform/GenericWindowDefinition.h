#pragma once

#include "Core/String/DoDoString.h"

namespace DoDo {
	/*enumeration to specify different window types for SWindows*/
	enum class EWindowType
	{
		/*value indicating that this is a standard, general-purpose window*/
		Normal,
		/*value indicating that this is a window used for a popup menu*/
		Menu,
		/*value indicating that this is a window used for a tooltop*/
		ToolTip,
		/*value indicating that this is a window used for a notification toast*/
		Notification,
		/*value indicating that this is a window used for a cursor decorator*/
		CursorDecorator,
		/*value indicating that this is a game window*/
		GameWindow
	};

	struct FGenericWindowDefinition
	{
		/*window type*/
		EWindowType m_type;

		/*the initially desired horizontal screen position*/
		float x_desired_position_on_screen;

		/*the initially desired vertical screen position*/
		float y_desired_position_on_screen;

		/*the initially desired width*/
		float m_width_desired_on_screen;

		/*the initially desired height*/
		float m_height_desired_on_screen;

		/*true if the window accepts input, false if the window is non-interactive*/
		bool m_accepts_input;

		/*the title of the window*/
		DoDoUtf8String m_title;
	};
}