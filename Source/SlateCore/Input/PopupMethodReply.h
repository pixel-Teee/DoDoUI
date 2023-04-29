#pragma once

#include "Core/Core.h"

namespace DoDo {

	enum class EPopupMethod : uint8_t
	{
		/*creating a new window allows us to place popups outside of the window in which the menu anchor resides*/
		CreateNewWindow,
		/*place the popup into the current window, applications that intend to run in fullscreen cannot create new windows, so they must use this method...*/
		UseCurrentWindow
	};

	enum class EShouldThrottle : uint8_t
	{
		No,
		Yes
	};
}