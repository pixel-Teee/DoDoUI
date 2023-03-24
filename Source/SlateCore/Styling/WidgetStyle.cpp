#include "PreCompileHeader.h"

#include "WidgetStyle.h"

#include "SlateColor.h"

namespace DoDo {
	const float FWidgetStyle::m_subdue_amount = 0.6f;

	FWidgetStyle& FWidgetStyle::set_foreground_color(const TAttribute<FSlateColor>& in_foreground)
	{
		m_foreground_color = in_foreground.Get().get_color(*this);

		m_subdued_foreground = m_foreground_color;
		m_subdued_foreground.A *= m_subdue_amount;

		return *this;
	}
}
