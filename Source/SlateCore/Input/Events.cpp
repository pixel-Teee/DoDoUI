#include "PreCompileHeader.h"

#include "Events.h"

#include "SlateCore/Layout/Geometry.h"//FGeometry depend on it

#include "SlateCore/Layout/WidgetPath.h"

namespace DoDo
{
	FGeometry FInputEvent::find_geometry(const std::shared_ptr<SWidget>& widget_to_find) const
	{
		return m_event_path->find_arranged_widget(widget_to_find).value_or(FArrangedWidget::get_null_widget()).m_geometry;
	}
	std::shared_ptr<SWindow> FInputEvent::get_window() const
	{
		return m_event_path->get_window();
	}
}
