#include "PreCompileHeader.h"

#include "SColorBlock.h"

#include "SlateCore/Rendering/RenderingCommon.h"//ESlateDrawEffect depends on it

#include "SlateCore/Rendering/DrawElements.h"//FSlateDrawElement depeneds on it

namespace DoDo {
	SLATE_IMPLEMENT_WIDGET(SColorBlock)
	void SColorBlock::Private_Register_Attributes(FSlateAttributeInitializer& attribute_initializer)
	{
		//todo:implement register
	}

	SColorBlock::SColorBlock()
		: m_color(*this, FLinearColor::White)
		, m_alpha_background_brush(*this, nullptr)
		, m_gradient_corner_radius(*this, glm::vec4(0.0f))
		, m_color_block_size(*this, glm::vec2(16, 16))
		, m_alpha_display_mode(*this, EColorBlockAlphaDisplayMode::Combined)
		, m_color_is_hsv(*this, false)
		, m_show_background_for_alpha(*this, false)
		, m_b_use_srgb(*this, true)
	{

	}

	void SColorBlock::Construct(const FArguments& in_args)
	{
		m_color.Assign(*this, in_args._Color);
		m_alpha_background_brush.Assign(*this, in_args._AlphaBackgroundBrush);
		m_gradient_corner_radius.Assign(*this, in_args._CornerRadius);
		m_color_block_size.Assign(*this, in_args._Size);
		m_mouse_button_down_handler = in_args._OnMouseButtonDown;
		m_alpha_display_mode.Assign(*this, in_args._AlphaDisplayMode);
		m_color_is_hsv.Assign(*this, in_args._ColorIsHSV);
		m_show_background_for_alpha.Assign(*this, in_args._ShowBackgroundForAlpha);
		m_b_use_srgb.Assign(*this, in_args._UseSRGB);
	}
	int32_t SColorBlock::On_Paint(const FPaintArgs& args, const FGeometry& allotted_geometry, const FSlateRect& my_culling_rect, FSlateWindowElementList& out_draw_elements, int32_t layer_id, const FWidgetStyle& in_widget_style, bool b_parent_enabled) const
	{
		const ESlateDrawEffect draw_effect = ESlateDrawEffect::None;

		EColorBlockAlphaDisplayMode display_mode = m_alpha_display_mode.Get();

		bool b_ignore_alpha = display_mode == EColorBlockAlphaDisplayMode::Ignore;

		FLinearColor in_color = m_color.Get();

		if (m_color_is_hsv.Get()) //color hsv will override the color
		{
			in_color = in_color.hsv_to_linear_rgb();
		}

		if (m_show_background_for_alpha.Get() && in_color.A < 1.0f && !b_ignore_alpha)
		{
			FSlateDrawElement::MakeBox(out_draw_elements, layer_id, allotted_geometry.to_paint_geometry(), m_alpha_background_brush.Get(), draw_effect);
		}

		std::vector<FSlateGradientStop> gradient_stops;

		switch (display_mode)
		{
		default:
		case EColorBlockAlphaDisplayMode::Combined:
			make_section(gradient_stops, glm::vec2(0.0f), allotted_geometry.get_local_size(), in_color, in_widget_style, false);
			break;
		case EColorBlockAlphaDisplayMode::Separate:
			make_section(gradient_stops, glm::vec2(0.0f), allotted_geometry.get_local_size() * 0.5f, in_color, in_widget_style, false);
			make_section(gradient_stops, allotted_geometry.get_local_size() * 0.5f, allotted_geometry.get_local_size(), in_color, in_widget_style, true);
			break;
		case EColorBlockAlphaDisplayMode::Ignore:
			make_section(gradient_stops, glm::vec2(0.0f), allotted_geometry.get_local_size(), in_color, in_widget_style, false);
			break;
		}

		//draw color
		FSlateDrawElement::make_gradient(out_draw_elements,
			layer_id + 1,
			allotted_geometry.to_paint_geometry(),
			std::move(gradient_stops),
			(allotted_geometry.get_local_size().x > allotted_geometry.get_local_size().y) ? Orient_Vertical : Orient_Horizontal,
			draw_effect,
			m_gradient_corner_radius.Get());

		return layer_id + 1;
	}
	FReply SColorBlock::On_Mouse_Button_On_Down(const FGeometry& my_geometry, const FPointerEvent& mouse_event)
	{
		if (m_mouse_button_down_handler.is_bound())
		{
			//if a handler is assigned, call it
			return m_mouse_button_down_handler.execute(my_geometry, mouse_event);
		}
		else
		{
			//otherwise the event is unhandled
			return FReply::un_handled();
		}
	}
	glm::vec2 SColorBlock::Compute_Desired_Size(float Layout_Scale_Multiplier) const
	{
		return m_color_block_size.Get();
	}
	void SColorBlock::make_section(std::vector<FSlateGradientStop>& out_gradient_stops, glm::vec2 start_pt, glm::vec2 end_pt, FLinearColor in_color, const FWidgetStyle& in_widget_style, bool b_ignore_alpha) const
	{
		//determine if it is hdr
		const float max_rgb = std::max(std::max(in_color.R, in_color.G), in_color.B);

		if (max_rgb > 1.0f)
		{
			const float alpha = b_ignore_alpha ? 1.0f : in_color.A;
			FLinearColor normalized_linear_color = in_color / max_rgb;
			normalized_linear_color.A = alpha;
			const FLinearColor draw_normalized_color = in_widget_style.get_color_and_opacity_tint() * normalized_linear_color.to_fcolor(m_b_use_srgb.Get());

			FLinearColor clamped_linear_color = in_color;
			clamped_linear_color.A = alpha * max_rgb;
			const FLinearColor draw_clamped_color = in_widget_style.get_color_and_opacity_tint() * clamped_linear_color.to_fcolor(m_b_use_srgb.Get());

			out_gradient_stops.push_back(FSlateGradientStop(start_pt, draw_normalized_color));
			out_gradient_stops.push_back(FSlateGradientStop((start_pt + end_pt) * 0.5f, draw_clamped_color));
			out_gradient_stops.push_back(FSlateGradientStop(end_pt, draw_normalized_color));
		}
		else
		{
			FColor draw_color = in_color.to_fcolor(m_b_use_srgb.Get());
			if (b_ignore_alpha)
			{
				draw_color.A = 255;
			}

			out_gradient_stops.push_back(FSlateGradientStop(start_pt, in_widget_style.get_color_and_opacity_tint() * draw_color));
			out_gradient_stops.push_back(FSlateGradientStop(end_pt, in_widget_style.get_color_and_opacity_tint() * draw_color));
		}
	}
}