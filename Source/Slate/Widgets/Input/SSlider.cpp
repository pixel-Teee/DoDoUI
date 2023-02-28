#include "PreCompileHeader.h"

#include "SSlider.h"

#include "SlateCore/Rendering/RenderingCommon.h"//ESlateDrawEffect

#include "SlateCore/Rendering/DrawElements.h"

namespace DoDo {
	SSlider::SSlider()
	{

	}
	void SSlider::Construct(const SSlider::FArguments& in_declaration)
	{
		m_style = in_declaration._Style;

		m_indent_handle = in_declaration._IndentHandle;
		m_b_mouse_uses_step = in_declaration._MouseUsesStep;
		m_b_required_controller_lock = in_declaration._RequiresControllerLock;
		m_locked_attribute = in_declaration._Locked;
		m_orientation = in_declaration._Orientation;
		m_step_size = in_declaration._StepSize;
		m_value_attribute = in_declaration._Value;
		m_min_value = in_declaration._MinValue;
		m_max_value = in_declaration._MaxValue;
		m_slider_bar_color = in_declaration._SliderBarColor;
		m_slider_handle_color = in_declaration._SliderHandleColor;
		m_b_is_focusable = in_declaration._IsFocusable;
		
		//todo:add event

		m_b_controller_input_captured = false;
	}

	float SSlider::get_normalized_value() const
	{
		if(m_max_value == m_min_value)
		{
			return 1.0f;
		}
		else
		{
			return (m_value_attribute.Get() - m_min_value) / (m_max_value - m_min_value);
		}
	}

	int32_t SSlider::On_Paint(const FPaintArgs& args, const FGeometry& allotted_geometry, const FSlateRect& my_culling_rect, FSlateWindowElementList& out_draw_elements, int32_t layer_id, const FWidgetStyle& in_widget_style, bool b_parent_enabled) const
	{
		//we draw the slider like a horizontal slider regardless of the orientation, and apply a render transform to make it display correctly
		//however, the allotted geometry is computed as it will be rendered, so we have to use the "horizontal orientation" when doing drawing computations

		const float allotted_width = m_orientation == Orient_Horizontal ? allotted_geometry.get_local_size().x : allotted_geometry.get_local_size().y;
		const float allotted_height = m_orientation == Orient_Vertical ? allotted_geometry.get_local_size().y : allotted_geometry.get_local_size().x;

		float handle_rotation;
		glm::vec2 handle_top_left_point;
		glm::vec2 slider_start_point;
		glm::vec2 slider_end_point;

		//calculate slider geometry as if it's a horizontal slider (we'll rotate it later if it's vertical)
		const glm::vec2 handle_size = get_thumb_image()->m_image_size;//todo:implement get thumb image size
		const glm::vec2 half_handle_size = 0.5f * handle_size;
		const float indentation = m_indent_handle.Get() ? handle_size.x : 0.0f;

		//we clamp to make sure that the slider cannot go out of the slider length
		const float slider_percent = glm::clamp(get_normalized_value(), 0.0f, 1.0f);
		const float slider_length = allotted_width - (indentation + handle_size.x);
		const float slider_handle_offset = slider_percent * slider_length;
		const float slider_y = 0.5f * allotted_height;

		handle_rotation = 0.0f;
		handle_top_left_point = glm::vec2(slider_handle_offset + (0.5f * indentation), slider_y - half_handle_size.y);

		slider_start_point = glm::vec2(half_handle_size.x, slider_y);
		slider_end_point = glm::vec2(allotted_width - half_handle_size.x, slider_y);

		FGeometry slider_geometry = allotted_geometry;

		//rotate the slider 90 deg if it's vertical, the 0 side goes on the bottom, the 1 side on the top
		if(m_orientation == Orient_Vertical)
		{
			//do this by translating along -x by the width of the geometry, then rotating 90 degress ccw(left-handle coordinate)
			
		}

		const bool b_enabled = should_be_enabled(b_parent_enabled);
		const ESlateDrawEffect draw_effects = b_enabled ? ESlateDrawEffect::None : ESlateDrawEffect::DisabledEffect;

		//draw slider bar
		auto bar_top_left = glm::vec2(slider_start_point.x, slider_start_point.y - m_style->m_bar_thick_ness * 0.5f);
		auto bar_size = glm::vec2(slider_end_point.x - slider_start_point.x, m_style->m_bar_thick_ness);

		auto bar_image = get_bar_image();
		auto thumb_image = get_thumb_image();
		FSlateDrawElement::MakeBox(out_draw_elements,
			layer_id,
			slider_geometry.to_paint_geometry(bar_top_left, bar_size),
			bar_image,
			draw_effects,
			bar_image->get_tint() * m_slider_bar_color.Get());//todo:fix me color

		++layer_id;

		//draw slider thumb
		FSlateDrawElement::MakeBox(out_draw_elements,
			layer_id,
			slider_geometry.to_paint_geometry(handle_top_left_point, get_thumb_image()->m_image_size),
			thumb_image,
			draw_effects,
			thumb_image->get_tint() * m_slider_handle_color.Get());//todo:fix me color

		//todo:draw
		return layer_id;
	}
	glm::vec2 SSlider::Compute_Desired_Size(float) const
	{
		static const glm::vec2 sslider_desired_size(16.0f, 16.0f);

		if (m_style == nullptr)
		{
			return sslider_desired_size;
		}

		const float thick_ness = std::max(m_style->m_bar_thick_ness, std::max(m_style->m_normal_thumb_image.m_image_size.y, m_style->m_hovered_thumb_image.m_image_size.y));

		if (m_orientation == Orient_Vertical)
		{
			return glm::vec2(thick_ness, sslider_desired_size.y);
		}

		return glm::vec2(sslider_desired_size.x, thick_ness);
	}
	const FSlateBrush* SSlider::get_bar_image() const
	{
		if (!Is_Enabled() || m_locked_attribute.Get())
		{
			return &m_style->m_disabled_bar_image;
		}
		else if (is_hovered())
		{
			return &m_style->m_hovered_bar_image;
		}
		else
		{
			return &m_style->m_normal_bar_image;
		}
	}
	const FSlateBrush* SSlider::get_thumb_image() const
	{
		if (!Is_Enabled() || m_locked_attribute.Get())
		{
			return &m_style->m_disabled_thumb_image;
		}
		else if (is_hovered())
		{
			return &m_style->m_hovered_thumb_image;
		}
		else
		{
			return &m_style->m_normal_thumb_image;
		}
	}
}
