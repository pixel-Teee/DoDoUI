#include <PreCompileHeader.h>

#include "WidgetPath.h"

namespace DoDo
{
	FWidgetPath::FWidgetPath()
	: m_widgets(EVisibility::visible)
	, m_top_level_window()
	, m_virtual_pointer_positions()
	{
	}

	bool FWidgetPath::is_valid() const
	{
		return m_widgets.num() > 0;
	}
}
