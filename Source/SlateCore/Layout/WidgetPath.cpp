#include <PreCompileHeader.h>

#include "WidgetPath.h"

#include <memory>

#include "SlateCore/Widgets/SWindow.h"

namespace DoDo
{
	FWidgetPath::FWidgetPath()
	: m_widgets(EVisibility::visible)
	, m_top_level_window()
	, m_virtual_pointer_positions()
	{
	}

	FWidgetPath::FWidgetPath(std::shared_ptr<SWindow> in_top_level_window, const FArrangedChildren& in_widget_path)
		: m_widgets(in_widget_path)
		, m_top_level_window(in_top_level_window)
		, m_virtual_pointer_positions()
	{
	}

	FWidgetPath::FWidgetPath(std::vector<FWidgetAndPointer>& in_widgets_and_pointers)
	: m_widgets(FArrangedChildren::hittest2_from_array(in_widgets_and_pointers))
	, m_top_level_window(in_widgets_and_pointers.size() > 0 ? std::reinterpret_pointer_cast<SWindow>(in_widgets_and_pointers[0].m_widget) : std::shared_ptr<SWindow>(nullptr))
	{
		//m_virtual_pointer_positions.resize(in_widgets_and_pointers.size());

		for(const FWidgetAndPointer& widget_and_pointer : in_widgets_and_pointers)
		{
			m_virtual_pointer_positions.push_back(widget_and_pointer.get_pointer_position());
		}
	}

	FWidgetPath FWidgetPath::get_path_down_to(std::shared_ptr<const SWidget> marker_widget) const
	{
		FArrangedChildren clipped_path(EVisibility::visible);

		bool b_copied_marker = false;
		for (int32_t widget_index = 0; !b_copied_marker && widget_index < m_widgets.num(); ++widget_index)//note:from top to down
		{
			clipped_path.add_widget(m_widgets[widget_index]);
			b_copied_marker = (m_widgets[widget_index].m_widget == marker_widget);
		}

		if (b_copied_marker)
		{
			//we found the marker widget and copied the path down to (and including) it
			return FWidgetPath(m_top_level_window, clipped_path);
		}
		else
		{
			//the marker widget was not in the widget path, we failed
			return FWidgetPath(nullptr, FArrangedChildren(EVisibility::visible));
		}
	}

	std::optional<FArrangedWidget> FWidgetPath::find_arranged_widget(
		std::shared_ptr<const SWidget> widget_to_find) const
	{
		for(int32_t widget_index = 0; widget_index < m_widgets.num(); ++widget_index)
		{
			if(m_widgets[widget_index].m_widget == widget_to_find)
			{
				return m_widgets[widget_index];
			}
		}

		return std::optional<FArrangedWidget>();
	}

	std::optional<FWidgetAndPointer> FWidgetPath::find_arranged_widget_and_cursor(
		std::shared_ptr<const SWidget> widget_to_find) const
	{
		const int32_t index = m_widgets.index_of_by_predicate([&widget_to_find](const FArrangedWidget& some_widget)
		{
			return some_widget.m_widget == widget_to_find;
		});

		return (index != -1) ? FWidgetAndPointer(m_widgets[index], m_virtual_pointer_positions[index]) : FWidgetAndPointer();
	}

	std::shared_ptr<SWindow> FWidgetPath::get_window() const
	{
		if (m_widgets.num() > 0)
		{
			std::shared_ptr<SWindow> first_widget_window = std::reinterpret_pointer_cast<SWindow>(m_widgets[0].m_widget);
			return first_widget_window;
		}
		else
		{
			return nullptr;
		}
	}

	bool FWidgetPath::is_valid() const
	{
		return m_widgets.num() > 0;
	}

	FWeakWidgetPath::FWeakWidgetPath(const FWidgetPath& in_widget_path)
		: m_window(in_widget_path.m_top_level_window)
	{
		for(int32_t widget_index = 0; widget_index < in_widget_path.m_widgets.num(); ++widget_index)
		{
			m_widgets.push_back(std::weak_ptr<SWidget>(in_widget_path.m_widgets[widget_index].m_widget));
		}
	}

	FWidgetPath FWeakWidgetPath::to_widget_path(EInterruptedPathHandling::Type interrupted_path_handling, const FPointerEvent* pointer_event, const EVisibility visibility_filter) const
	{
		FWidgetPath widget_path;
		to_widget_path(widget_path, interrupted_path_handling, pointer_event, visibility_filter);
		return widget_path;
	}

	std::shared_ptr<FWidgetPath> FWeakWidgetPath::to_widget_path_ptr(EInterruptedPathHandling::Type interrupted_path_handling, const FPointerEvent* pointer_event, const EVisibility visibility_filter) const
	{
		std::shared_ptr<FWidgetPath> widget_path = std::make_shared<FWidgetPath>();

		to_widget_path(*widget_path, interrupted_path_handling, pointer_event, visibility_filter);

		return widget_path;
	}

	FWeakWidgetPath::EPathResolutionResult::Result FWeakWidgetPath::to_widget_path(FWidgetPath& widget_path,
		EInterruptedPathHandling::Type interrupted_path_handling, const FPointerEvent* pointer_event,
		const EVisibility visiblity_filter) const
	{
		std::vector<std::shared_ptr<SWidget>> widget_ptrs;

		//covert the weak pointers into shared pointers because we are bout to do something with this path instead of just observe it
		std::shared_ptr<SWindow> top_level_window_ptr = m_window.lock();//todo:check?

		int32_t path_size = 0;
		for(std::vector<std::weak_ptr<SWidget>>::const_iterator some_weak_widget_ptr = m_widgets.begin(); some_weak_widget_ptr != m_widgets.end(); ++some_weak_widget_ptr)
		{
			const int32_t max_widget_path = 1000;
			++path_size;

			if(path_size < max_widget_path)
			{
				widget_ptrs.push_back(some_weak_widget_ptr->lock());
			}
			else
			{
				widget_path = FWidgetPath();
				return EPathResolutionResult::Truncated;//todo:how to understand this?
			}
		}

		//todo:implement fast path widget logic

		//todo:implement slow path widget logic
		{
			std::vector<FWidgetAndPointer> path_with_geometries;

			//the path can get interrupted if some subtree of widgets disappeared, but we still maintain weak references to it
			bool b_path_un_interrupted = false;

			//for each widget in the path compute the geometry, we are able to do this starting the top-level window because it knows its own geometry
			if (top_level_window_ptr)
			{
				b_path_un_interrupted = true;

				FGeometry parent_geometry = top_level_window_ptr->get_window_geometry_in_window();

				path_with_geometries.push_back(FWidgetAndPointer(FArrangedWidget(top_level_window_ptr, parent_geometry)));

				FArrangedChildren arranged_children(visiblity_filter, true);

				std::optional<FVirtualPointerPosition> virtual_pointer_pos;

				//for every widget in the vertical size
				for (int32_t widget_index = 0; b_path_un_interrupted && widget_index < widget_ptrs.size() - 1; ++widget_index)
				{
					std::shared_ptr<SWidget> cur_widget = widget_ptrs[widget_index];

					bool b_found_child = false;
					if (cur_widget)
					{
						//arrange the widget's children to find their geometries
						arranged_children.empty();

						const bool b_update_visibility_attributes = true;
						cur_widget->Arrange_Children(parent_geometry, arranged_children, b_update_visibility_attributes);

						//find the next widget in the path among the arranged children
						for (int32_t search_index = 0; !b_found_child && search_index < arranged_children.num(); ++search_index)
						{
							FArrangedWidget& arranged_widget = arranged_children[search_index];

							if (arranged_widget.m_widget == widget_ptrs[widget_index + 1])
							{
								if (pointer_event && !virtual_pointer_pos.has_value())
								{
									//todo:implement translate mouse coordinate for custom hit test child
								}

								b_found_child = true;

								//remember the widget, the associated geometry, and the pointer position in a transformed space
								path_with_geometries.push_back(FWidgetAndPointer(arranged_children[search_index], virtual_pointer_pos));

								//the next child in the vertical slice will be arranged with respect to its parent's geometry
								parent_geometry = arranged_children[search_index].m_geometry;
							}
						}
					}

					b_path_un_interrupted = b_found_child;
					if (!b_found_child && interrupted_path_handling == EInterruptedPathHandling::ReturnInvalid)
					{
						return EPathResolutionResult::Truncated;
					}
				}
			}

			widget_path = FWidgetPath(path_with_geometries);
			return b_path_un_interrupted ? EPathResolutionResult::Live : EPathResolutionResult::Truncated;
		}
	}
	bool FWeakWidgetPath::contains_widget(const SWidget* widget_to_find) const
	{
		for (int32_t widget_index = 0; widget_index < m_widgets.size(); ++widget_index)
		{
			if (m_widgets[widget_index].lock().get() == widget_to_find)
			{
				return true;
			}
		}

		return false;
	}
}
