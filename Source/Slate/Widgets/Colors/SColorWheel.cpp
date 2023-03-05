#include "PreCompileHeader.h"

#include "SColorWheel.h"

#include "SlateCore/Styling/CoreStyle.h"//FCoreStyle depends on it

#include "SlateCore/Styling/ISlateStyle.h"

#include "SlateCore/Rendering/RenderingCommon.h"//ESlateDrawEffect depends on it

#include "SlateCore/Rendering/DrawElements.h"//FSlateDrawElement depends on it

namespace DoDo {
	SColorWheel::SColorWheel()
	{
		
	}


	void SColorWheel::Construct(const FArguments& in_args)
	{
		m_image = FCoreStyle::get().get_brush("ColorWheel.HueValueCircle");
		m_selector_image = FCoreStyle::get().get_brush("ColorWheel.Selector");
	}

	int32_t SColorWheel::On_Paint(const FPaintArgs& args, const FGeometry& allotted_geometry, const FSlateRect& my_culling_rect, FSlateWindowElementList& out_draw_elements, int32_t layer_id, const FWidgetStyle& in_widget_style, bool b_parent_enabled) const
	{
		const bool b_is_enabled = should_be_enabled(b_parent_enabled);

		const ESlateDrawEffect draw_effects = b_is_enabled ? ESlateDrawEffect::None : ESlateDrawEffect::DisabledEffect;

		const glm::vec2& selector_size = m_selector_image->m_image_size;

		const glm::vec2 circle_size = allotted_geometry.get_local_size() - selector_size;

		FSlateDrawElement::MakeBox(out_draw_elements,
			layer_id,
			allotted_geometry.to_paint_geometry(0.5f * selector_size, circle_size),
			m_image,
			draw_effects,
			m_image->get_tint());

		//todo:implement calc relative position from center
		FSlateDrawElement::MakeBox(out_draw_elements,
			layer_id + 1,
			allotted_geometry.to_paint_geometry(0.5f * (allotted_geometry.get_local_size() - selector_size), selector_size),
			m_selector_image,
			draw_effects,
			m_selector_image->get_tint());

		return layer_id + 1;
	}

	glm::vec2 SColorWheel::Compute_Desired_Size(float) const
	{
		return glm::vec2();
	}

}