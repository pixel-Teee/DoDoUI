#include <PreCompileHeader.h>

#include "SButton.h"

namespace DoDo
{
	int32_t SButton::On_Paint(const FPaintArgs& args, const FGeometry& allotted_geometry,
		const FSlateRect& my_culling_rect, FSlateWindowElementList& out_draw_elements, int32_t layer_id,
		const FWidgetStyle& in_widget_style, bool b_parent_enabled) const
	{
		//return SBorder::On_Paint(args, allotted_geometry, my_culling_rect, out_draw_elements, layer_id, in_widget_style,
		//                         b_parent_enabled);

		//todo:implement should be enabled

		//todo:implement get show disabled effect

		//todo:implement get border image

		//todo:implement ESlateDrawEffect

		//todo:implement FSlateDrawElement::MakeBox
		return SCompoundWidget::On_Paint(args, allotted_geometry, my_culling_rect, out_draw_elements, layer_id, in_widget_style, b_parent_enabled);
	}
}
