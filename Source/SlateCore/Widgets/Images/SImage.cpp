#include <PreCompileHeader.h>

#include "SImage.h"

#include "SlateCore/Rendering/RenderingCommon.h"

#include "SlateCore/Rendering/DrawElements.h"

namespace DoDo {
	void SImage::Private_Register_Attributes(FSlateAttributeInitializer& attribute_initializer)
	{

	}

	SImage::SImage()
		: m_image_attribute(*this)
		, m_color_and_opacity_attribute(*this)
		, m_desired_size_override_attribute(*this)
	{

	}
	void SImage::Construct(const FArguments& in_args)
	{
		m_image_attribute.Assign(*this, in_args._Image);
		m_color_and_opacity_attribute.Assign(*this, in_args._ColorAndOpacity);
		m_b_flip_for_right_to_left_flow_direction = in_args._FlipForRightToLeftFlowDirection;

		//m_desired_size_override_attribute.Assign(*this, in_args._)
	}
	glm::vec2 SImage::Compute_Desired_Size(float) const
	{
		const FSlateBrush* image_brush = m_image_attribute.Get();
		if (image_brush != nullptr)
		{
			const std::optional<glm::vec2>& current_size_override = m_desired_size_override_attribute.Get();

			return current_size_override.has_value() ? current_size_override.value() : image_brush->m_image_size;
		}

		return glm::vec2(0.0f);
	}
	int32_t SImage::On_Paint(const FPaintArgs& args, const FGeometry& allotted_geometry, const FSlateRect& my_culling_rect, 
		FSlateWindowElementList& out_draw_elements, int32_t layer_id, const FWidgetStyle& in_widget_style, bool b_parent_enabled) const
	{
		const FSlateBrush* image_brush = m_image_attribute.Get();

		if ((image_brush != nullptr) && (image_brush->m_draw_as != ESlateBrushDrawType::NoDrawType))
		{
			const bool b_is_enabled = should_be_enabled(b_parent_enabled);

			const ESlateDrawEffect draw_effects = b_is_enabled ? ESlateDrawEffect::None : ESlateDrawEffect::DisabledEffect;

			//todo:fix me
			const glm::vec4 final_color_and_opacity(m_color_and_opacity_attribute.Get());

			if (m_b_flip_for_right_to_left_flow_direction && g_slate_flow_direction == EFlowDirection::RightToLeft)
			{
				const FGeometry flipped_geometry = allotted_geometry.make_child(FSlateRenderTransform(glm::vec2(-1.0f, -1.0f)));
				FSlateDrawElement::MakeBox(out_draw_elements, layer_id, flipped_geometry.to_paint_geometry(), image_brush, draw_effects, final_color_and_opacity);
			}
			else
			{
				FSlateDrawElement::MakeBox(out_draw_elements, layer_id, allotted_geometry.to_paint_geometry(), image_brush, draw_effects, final_color_and_opacity);
			}
		}

		return layer_id;
	}
}