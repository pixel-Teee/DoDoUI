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

	/*
	 * just like a widget path, but uses weak pointers and does not store geometry
	 */
	class FWeakWidgetPath
	{
	public:
		/*construct a weak widget path from a widget path, defaults to an invalid path*/
		FWeakWidgetPath(const FWidgetPath& in_widget_path = FWidgetPath());

		/*should interrupted paths truncate or return an invalid path?*/
		struct EInterruptedPathHandling
		{
			enum Type
			{
				Truncate,
				ReturnInvalid
			};
		};

		struct EPathResolutionResult
		{
			enum Result
			{
				Live,
				Truncated
			};
		};

		/*
		 * make a non-weak widget path out of this weak widget path, do this by computing all the relevant geometries and coverting the weak pointers to TSharedPtr
		 *
		 * @param WidgetPath the non-weak is returned via this
		 * @param interrupted path handling should interrupted paths result in a truncated or an invalid path
		 * @return whether the path is truncated or live - a live path refers to a widget that is currently active and visible, a widget with a truncated path is not
		 */
		EPathResolutionResult::Result to_widget_path(FWidgetPath& widget_path, EInterruptedPathHandling::Type interrupted_path_handling = EInterruptedPathHandling::Truncate, const FPointerEvent* pointer_event = nullptr,
			const EVisibility visiblity_filter = EVisibility::visible) const;

		std::vector<std::weak_ptr<SWidget>> m_widgets;

		std::weak_ptr<SWindow> m_window;
	};
}
