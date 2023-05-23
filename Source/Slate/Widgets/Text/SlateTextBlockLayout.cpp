#include "PreCompileHeader.h"

#include "SlateTextBlockLayout.h"

#include "Slate/Framework/Text/SlateTextLayout.h"//FSlateTextLayout depends on it

namespace DoDo {
	FSlateTextBlockLayout::FSlateTextBlockLayout()
	{
	}
	FSlateTextBlockLayout::FSlateTextBlockLayout(SWidget* in_owner, FTextBlockStyle in_default_text_style, const std::optional<ETextShapingMethod> in_text_shaping_method)
		: m_text_layout(FSlateTextLayout::Create(in_owner, std::move(in_default_text_style)))
	{
	}
	glm::vec2 FSlateTextBlockLayout::get_desired_size() const
	{
		return m_text_layout->get_size();
	}
	int32_t FSlateTextBlockLayout::On_Paint(const FPaintArgs& in_paint_args, const FGeometry& in_allotted_geometry, const FSlateRect& in_clipping_rect, FSlateWindowElementList& out_draw_elements, int32_t layer_id, const FWidgetStyle& in_widget_style, bool b_parent_enabled)
	{
		return m_text_layout->On_Paint(in_paint_args, in_allotted_geometry, in_clipping_rect, out_draw_elements, layer_id, in_widget_style, b_parent_enabled);
	}
}