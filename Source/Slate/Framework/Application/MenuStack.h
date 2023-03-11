#pragma once

namespace DoDo {
	/*
	* represents a stack of open menus
	* the last item in the stack is the top most menu
	* menus are described as IMenus, implementations of IMenu can control how the menus are created and presented (e.g. in their own window)
	*/
	class FMenuStack
	{
	public:
		/*constructor*/
		FMenuStack()
			: m_b_host_window_guard(false)
		{

		}

		/*guard to prevent the host window and host window popup panel being set reentrantly*/
		bool m_b_host_window_guard;
	};
}