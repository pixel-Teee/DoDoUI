#pragma once

#include "Core/Core.h"

#include <vector>

namespace DoDo
{
	class SWindow;
	class Window;
	/*implements a manager for slate windows*/
	class FSlateWindowHelper
	{
	public:

		/*
		 * searches the given collection of windows to find the slate window that corresponds to the specified platform window
		 *
		 * @param WindowsToSearch the collection of windows to search
		 * @param PlatformWindow the platform window to find the slate window for
		 * @return the corresponding slate window, or nullptr if it was not found
		 */
		static std::shared_ptr<SWindow> find_window_by_platform_window(const std::vector<std::shared_ptr<SWindow>>& windows_to_search, const std::shared_ptr<Window>& platform_window);

		/*
		* removes the specified window from the given collection of windows
		* 
		* @param Windows the collection of windows
		* @param WindowToRemove the window to remove
		*/
		static void remove_window_from_list(std::vector<std::shared_ptr<SWindow>>& windows, const std::shared_ptr<SWindow>& window_to_remove);
	};
}