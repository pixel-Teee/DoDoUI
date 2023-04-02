#include <PreCompileHeader.h>

#include "SlateWindowHelper.h"

#include "SlateCore/Widgets/SWindow.h"

namespace DoDo
{
	std::shared_ptr<SWindow> FSlateWindowHelper::find_window_by_platform_window(
		const std::vector<std::shared_ptr<SWindow>>& windows_to_search, const std::shared_ptr<Window>& platform_window)
	{
		for(int32_t window_index = 0; window_index < windows_to_search.size(); ++window_index)
		{
			std::shared_ptr<SWindow> some_window = windows_to_search[window_index];

			std::shared_ptr<Window> some_native_window = some_window->get_native_window();

			if(some_native_window == platform_window)
			{
				return some_window;
			}

			//search child windows
			std::shared_ptr<SWindow> found_child_window = find_window_by_platform_window(some_window->get_child_windows(), platform_window);

			if(found_child_window)
			{
				return found_child_window;
			}
		}

		return nullptr;
	}
	void FSlateWindowHelper::remove_window_from_list(std::vector<std::shared_ptr<SWindow>>& windows, const std::shared_ptr<SWindow>& window_to_remove)
	{
		auto it = std::find(windows.begin(), windows.end(), window_to_remove);

		//if (it != windows.end())
		//{
		//	for (int32_t child_index = 0; child_index < windows.size(); ++child_index)
		//	{
		//		remove_window_from_list(windows[child_index]->get_child_windows(), window_to_remove);
		//	}
		//}

		windows.erase(it);
	}
}
