#pragma once

#include <memory>

#include <vector>

#include <optional>//optional

#include "ArrangedChildren.h"

#include "SlateCore/Input/Events.h"//FVirtualPointerPosition

namespace DoDo
{
	class SWindow;
	/*
	 * a widget path is a vertical slice through the tree
	 * the canonical form for widget paths is "leafmost last", the top-level window always resides at index 0
	 * a widget path also contains a reference to a top-level SWindow that contains all the widgets in the path
	 * the window is needed for it's ability to determine it's own geometry, from which the geometries of the rest
	 * of the widget can be determined
	 */
	class FWidgetPath
	{
	public:
		FWidgetPath();

		FWidgetPath(std::vector<FWidgetAndPointer>& in_widgets_and_pointers);

		//FWidgetPath(const FWidgetPath&) = delete;

		//FWidgetPath(FWidgetPath&&) = delete;

		bool is_valid() const;

		/*get the virtual representation of the mouse at each level in the widget path*/
		std::optional<FVirtualPointerPosition> get_virtual_pointer_position(int32_t index) const
		{
			return m_virtual_pointer_positions[index];
		}
	public:
		/*the widgets that make up the widget path, the first item is the root widget, the end is the widget this path was built for*/
		FArrangedChildren m_widgets;


		/*the top level window of this widget path*/
		std::shared_ptr<SWindow> m_top_level_window;

		/*the virtual representation of the mouse at each level in the widget path*/
		std::vector<std::optional<FVirtualPointerPosition>> m_virtual_pointer_positions;
	};
}
