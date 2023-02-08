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

	bool FWidgetPath::is_valid() const
	{
		return m_widgets.num() > 0;
	}
}
