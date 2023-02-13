#include <PreCompileHeader.h>

#include "ArrangedChildren.h"

#include "SlateCore/Widgets/SWidget.h"

namespace DoDo
{
	void FArrangedChildren::add_widget(const FArrangedWidget& in_widget_geometry)
	{
		add_widget(in_widget_geometry.m_widget->get_visibility(), in_widget_geometry);
	}
}
