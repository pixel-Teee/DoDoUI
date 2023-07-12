#include "PreCompileHeader.h"

#include "SScrollBar.h"

namespace DoDo {
	void SScrollBar::Construct(const FArguments& in_args)
	{
		set_style(in_args._Style);


	}
	void SScrollBar::set_style(const FScrollBarStyle* in_style)
	{
		const FScrollBarStyle* style = in_style;

		if (style == nullptr)
		{
			FArguments defaults;
			style = defaults._Style;
		}

		m_normal_thumb_image = &style->m_normal_thumb_image;
		m_hovered_thumb_image = &style->m_hovered_thumb_image;
		m_dragged_thumb_image = &style->m_dragged_thumb_image;

		if (m_orientation == Orient_Vertical)
		{
			m_background_brush = &style->m_horizontal_background_image;
			m_top_brush = &style->m_horizontal_top_slot_image;
			m_bottom_brush = &style->m_horizontal_bottom_slot_image;
		}
	}
	float SScrollBar::distance_from_top() const
	{
		return 0.0f;
	}
	float SScrollBar::distance_from_bottom() const
	{
		return 0.0f;
	}
}