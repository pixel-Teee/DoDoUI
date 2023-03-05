#include <PreCompileHeader.h>

#include "WidgetPath.h"

#include <memory>

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

	std::shared_ptr<SWindow> FWidgetPath::get_window() const
	{
		std::shared_ptr<SWindow> first_widget_window = std::reinterpret_pointer_cast<SWindow>(m_widgets[0].m_widget);

		return first_widget_window;
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

		std::vector<FWidgetAndPointer> path_with_geometries;

		//todo:implement slow path widget logic

		return EPathResolutionResult::Truncated;
	}
}
