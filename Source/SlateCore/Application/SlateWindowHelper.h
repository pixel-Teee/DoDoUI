#pragma once

#include "Core/Core.h"

#include <vector>

#include "SlateCore/Layout/Visibility.h"//EVisibility

namespace DoDo
{
	class SWindow;
	class Window;
	class SWidget;
	class FWidgetPath;
	struct EVisibility;
	/*implements a manager for slate windows*/
	class FSlateWindowHelper
	{
	public:
		/*
		* recorders the given collection of windows so the specified window is brought to the front
		* 
		* @param Windows the collection of windows to reorder
		* @param WindowToBringToFront the window to bring to the front
		*/
		static void arrange_window_to_front(std::vector<std::shared_ptr<SWindow>>& windows, const std::shared_ptr<SWindow>& window_to_bring_to_front);

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

		/*
		* put 'bring me to front' at the front of the list of 'windows to reorder'
		* the top-most (front-most) window is last in z-order and therefore is added to the end of the list
		* 
		* @param Windows an ordered list of windows
		* @param BringMeToFront the window to bring to front
		*/
		static void bring_window_to_front(std::vector<std::shared_ptr<SWindow>>& windows, const std::shared_ptr<SWindow>& bring_me_to_front);

		/*
		* searches for the specified widget and generates a full path to it
		* 
		* note: this is a relatively slow operation!
		* 
		* @param in widget widget to generate a path to
		* @param OutWidgetPath the generated widget path
		* @param VisibilityFilter widgets must have this type of visibility to be included the path
		* @return true if the widget was found
		*/
		static bool find_path_to_widget(const std::vector<std::shared_ptr<SWindow>>& windows_to_search, std::shared_ptr<const SWidget> in_widget, FWidgetPath& out_widget_path, EVisibility visibility_filter = EVisibility::visible);

	private:
		/*
		* make bring me to front first among its peers
		* i.e. make it the last window in its parent's list of child windows
		* 
		* @return the top-most window whose children were re-arranged
		*/
		static std::shared_ptr<SWindow> bring_to_front_in_parent(const std::shared_ptr<SWindow>& window_to_bring_to_front);
	};
}