#include <PreCompileHeader.h>

#include "SlateWindowHelper.h"

#include "SlateCore/Widgets/SWindow.h"

#include "SlateCore/Layout/ArrangedChildren.h"//FArrangedChildren depends on it

#include "SlateCore/Layout/WidgetPath.h"//FWidgetPath

namespace DoDo
{
	void FSlateWindowHelper::arrange_window_to_front(std::vector<std::shared_ptr<SWindow>>& windows, const std::shared_ptr<SWindow>& window_to_bring_to_front)
	{
		auto it = std::find(windows.begin(), windows.end(), window_to_bring_to_front);

		if (it != windows.end())
			windows.erase(it);

		if ((windows.size() == 0)) //todo:check is topmost window
		{
			windows.push_back(window_to_bring_to_front);
		}
		else
		{
			bool performed_insert = false;

			int32_t window_index = windows.size() - 1;
			//todo:skip non-regular windows

			windows.insert(windows.begin() + window_index + 1, window_to_bring_to_front);
		}
	}
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
	void FSlateWindowHelper::bring_window_to_front(std::vector<std::shared_ptr<SWindow>>& windows, const std::shared_ptr<SWindow>& bring_me_to_front)
	{
		const std::shared_ptr<SWindow> top_level_window_to_reorder = bring_to_front_in_parent(bring_me_to_front);

		FSlateWindowHelper::arrange_window_to_front(windows, top_level_window_to_reorder);
	}
	bool FSlateWindowHelper::find_path_to_widget(const std::vector<std::shared_ptr<SWindow>>& windows_to_search, std::shared_ptr<const SWidget> in_widget, FWidgetPath& out_widget_path, EVisibility visibility_filter)
	{
		//iterate over our top level windows
		bool b_found_widget = false;

		for (int32_t window_index = 0; !b_found_widget && window_index < windows_to_search.size(); ++window_index)
		{
			//make a widget path that contains just the top-level window
			std::shared_ptr<SWindow> current_window = windows_to_search[window_index];

			FArrangedChildren just_window(visibility_filter);
			{
				just_window.add_widget(FArrangedWidget(current_window, current_window->get_window_geometry_in_screen()));
			}

			FWidgetPath path_to_widget(current_window, just_window);

			//attempt to extend it to the desired child widget, essentially a full-window search for in widget
			//if ((current_window == in_widget || path_to_widget.extend_path_to(, visibility_filter))) //todo:Add matcher
			//{
			//	out_widget_path = path_to_widget;
			//
			//	b_found_widget = true;
			//}
			
			//todo:child windows
		}

		return b_found_widget;
	}
	std::shared_ptr<SWindow> FSlateWindowHelper::bring_to_front_in_parent(const std::shared_ptr<SWindow>& window_to_bring_to_front)
	{
		const std::shared_ptr<SWindow> parent_window = window_to_bring_to_front->get_parent_window();

		if (!parent_window)
		{
			return window_to_bring_to_front;
		}

		FSlateWindowHelper::arrange_window_to_front(parent_window->get_child_windows(), window_to_bring_to_front);

		return bring_to_front_in_parent(parent_window);
	}
}
