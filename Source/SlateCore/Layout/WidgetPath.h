#pragma once

#include <memory>

#include <vector>

#include <optional>//optional

#include "ArrangedChildren.h"

#include "SlateCore/Input/Events.h"//FVirtualPointerPosition

#include "SlateCore/Types/SlateEnums.h"//EUINavigation depends on it

namespace DoDo
{
	class SWindow;

	/*matches widgets against in widget*/
	struct FWidgetMatcher
	{
		FWidgetMatcher(const std::shared_ptr<const SWidget> in_widget)
			: m_widget_to_find(in_widget)
		{}

		bool is_match(const std::shared_ptr<const SWidget>& in_widget) const
		{
			return m_widget_to_find == in_widget;
		}

		std::shared_ptr<const SWidget> m_widget_to_find;
	};
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

		FWidgetPath(std::shared_ptr<SWindow> in_top_level_window, const FArrangedChildren& in_widget_path);

		FWidgetPath(std::vector<FWidgetAndPointer>& in_widgets_and_pointers);

		/*
		* @param MarkerWidget copy the path up to and including this widget
		* 
		* @return a copy of the widget path down to and including the MarkerWidget, if the MarkerWidget is not found in the path, return an invalid path
		*/
		FWidgetPath get_path_down_to(std::shared_ptr<const SWidget> marker_widget) const;
			 
		//FWidgetPath(const FWidgetPath&) = delete;

		//FWidgetPath(FWidgetPath&&) = delete;

		std::optional<FArrangedWidget> find_arranged_widget(std::shared_ptr<const SWidget> widget_to_find) const;

		std::optional<FWidgetAndPointer> find_arranged_widget_and_cursor(std::shared_ptr<const SWidget> widget_to_find) const;

		/*
		* get the first (top-most) widget in this path, which is always a window, assumes path is valid
		* 
		* @return window at the top of this path
		*/
		std::shared_ptr<SWindow> get_window() const;

		bool is_valid() const;

		/*get the last (leaf-most) widget in this path, assumes path is valid*/
		std::shared_ptr<SWidget> get_last_widget() const
		{
			return m_widgets[m_widgets.num() - 1].m_widget;
		}

		/*get the virtual representation of the mouse at each level in the widget path*/
		std::optional<FVirtualPointerPosition> get_virtual_pointer_position(int32_t index) const
		{
			return m_virtual_pointer_positions[index];
		}

		/*
		* extend the current path such that it reaches some widget that qualifies as a Match
		* the widget to match must be a descendant of the last widget currently in the path
		* 
		* @param Matcher some struct that has a "bool is_match(const std::shared_ptr<const SWidget>& in_widget) const" method
		* @param visibility filter widgets must have this type of visibility to be included the path
		* 
		* @return true if succcessful
		*/
		template<typename MatcherType>
		bool extend_path_to(const MatcherType& matcher, EVisibility visibility_filter = EVisibility::visible)
		{
			const FArrangedWidget& last_widget = m_widgets.last();

			FArrangedChildren extension = generate_path_to_widget(matcher, last_widget, EUINavigation::Next, visibility_filter);

			for (int32_t widget_index = 0; widget_index < extension.num(); ++widget_index)
			{
				this->m_widgets.add_widget(extension[widget_index]);
			}

			return extension.num() > 0;
		}

		/*
		* generate a path from From Widget to Widget To Find, the path will not include from widget
		* 
		* @param Matcher some struct that has a "bool is_match(const std::shared_ptr<const SWidget>& in_widget) const" method
		* @param from widget widget from which we a building a path, add item
		* @param visibility filter widgets must have this type of visibility to be included the path
		* @return a path from from widget to widget to find, will not include from widget
		*/
		template<typename MatcherType>
		FArrangedChildren generate_path_to_widget(const MatcherType& matcher, const FArrangedWidget& from_widget,
			EUINavigation navigation_type = EUINavigation::Next, EVisibility visibility_filter = EVisibility::visible)
		{
			FArrangedChildren path_result(visibility_filter);

			if (navigation_type == EUINavigation::Next)
			{
				search_for_widget_recursively(matcher, from_widget, path_result, visibility_filter);
			}
			else
			{
				//todo:fix this
			}

			//reverse the list of widgets we found, canonical from is leaf most last
			path_result.reverse();

			return path_result;
		}
	private:
		/*
		* utility function to search recursively through a widget hierarchy for a specific widget
		*/
		template<typename MatchRuleType>
		static bool search_for_widget_recursively(const MatchRuleType& match_rule, const FArrangedWidget& in_candidate, FArrangedChildren& out_reversed_path, EVisibility visibility_filter = EVisibility::visible);
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
		* make a non-weak widget path out of this weak widget path, do this by computing all the relevant geometries and converting the weak pointers to TSharedPtr
		* 
		* @param InterruptedPathHandling should interrupted paths result in a truncated path or an invalid path
		*/
		FWidgetPath to_widget_path(EInterruptedPathHandling::Type interrupted_path_handling = EInterruptedPathHandling::Truncate, const FPointerEvent* pointer_event = nullptr,
			const EVisibility visibility_filter = EVisibility::visible) const;

		std::shared_ptr<FWidgetPath> to_widget_path_ptr(EInterruptedPathHandling::Type interrupted_path_handling = EInterruptedPathHandling::Truncate, const FPointerEvent* pointer_event = nullptr,
			const EVisibility visibility_filter = EVisibility::visible) const;

		/*
		 * make a non-weak widget path out of this weak widget path, do this by computing all the relevant geometries and coverting the weak pointers to TSharedPtr
		 *
		 * @param WidgetPath the non-weak is returned via this
		 * @param interrupted path handling should interrupted paths result in a truncated or an invalid path
		 * @return whether the path is truncated or live - a live path refers to a widget that is currently active and visible, a widget with a truncated path is not
		 */
		EPathResolutionResult::Result to_widget_path(FWidgetPath& widget_path, EInterruptedPathHandling::Type interrupted_path_handling = EInterruptedPathHandling::Truncate, const FPointerEvent* pointer_event = nullptr,
			const EVisibility visiblity_filter = EVisibility::visible) const;

		/*get the last (leaf-most) widget in this path, assumes path is valid*/
		std::weak_ptr<SWidget> get_last_widget() const
		{
			return m_widgets.back();
		}

		/*@return true if the widget to find is in this widget path, false otherwise*/
		bool contains_widget(const SWidget* widget_to_find) const;

		/*a valid path has at least one widget in it*/
		bool is_valid() const { return m_widgets.size() > 0; }

		std::vector<std::weak_ptr<SWidget>> m_widgets;

		std::weak_ptr<SWindow> m_window;
	};

	//todo:add widget path impl
	template<typename MatchRuleType>
	inline bool FWidgetPath::search_for_widget_recursively(const MatchRuleType& match_rule, const FArrangedWidget& in_candidate, FArrangedChildren& out_reversed_path, EVisibility visibility_filter)
	{
		const bool b_allow_3d_widgets = true;
		const bool b_update_visibility_attributes = true;
		FArrangedChildren arranged_children(visibility_filter, b_allow_3d_widgets);
		in_candidate.m_widget->Arrange_Children(in_candidate.m_geometry, arranged_children, b_update_visibility_attributes);

		for (int32_t child_index = 0; child_index < arranged_children.num(); ++child_index)
		{
			const FArrangedWidget& some_child = arranged_children[child_index];
			if(match_rule.is_match(some_child.m_widget))
			{
				out_reversed_path.add_widget(some_child);
				return true;
			}
			else if (search_for_widget_recursively(match_rule, some_child, out_reversed_path, visibility_filter))
			{
				out_reversed_path.add_widget(some_child);
				return true;
			}
		}

		return false;
	}
}
