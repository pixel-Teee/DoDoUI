#include <PreCompileHeader.h>

#include "SNodePanel.h"

#include "SlateCore/Styling/SlateBrush.h"//FSlateBrush depends on it

namespace DoDo {
	SNodePanel::SNodePanel()
	{
	}

	void SNodePanel::paint_background_as_lines(const FSlateBrush* background_image, const FGeometry& allotted_geometry, const FSlateRect& my_culling_rect, FSlateWindowElementList& out_draw_elements, int32_t& draw_layer_id) const
	{
		const bool b_antialias = false;

		const FLinearColor graph_background_image_color(background_image->m_tint_color.get_specified_color());

	}
}