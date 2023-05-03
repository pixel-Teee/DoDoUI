#include "PreCompileHeader.h"

#include "TextLayout.h"

#include "SlateCore/Layout/Children.h"//TSlotlessChildren depends on it

namespace DoDo {
	ETextJustify::Type FTextLayout::calculate_line_view_visual_justification(const FLineView& line_view) const
	{
		//work out the visual justification to use for this line
		ETextJustify::Type visual_justification = m_justification;

		if (line_view.m_text_base_direction == TextBiDi::ETextDirection::RightToLeft)
		{
			if (visual_justification == ETextJustify::Left)
			{
				visual_justification = ETextJustify::Right;
			}
			else if (visual_justification == ETextJustify::Right)
			{
				visual_justification = ETextJustify::Left;
			}
		}

		return visual_justification;
	}
}