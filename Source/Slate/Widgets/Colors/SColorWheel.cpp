#include "PreCompileHeader.h"

#include "SColorWheel.h"

#include "SlateCore/Styling/CoreStyle.h"//FCoreStyle depends on it

#include "SlateCore/Styling/ISlateStyle.h"

#include "SlateCore/Rendering/RenderingCommon.h"//ESlateDrawEffect depends on it

#include "SlateCore/Rendering/DrawElements.h"//FSlateDrawElement depends on it

namespace DoDo {
	SColorWheel::SColorWheel()
		: m_selected_color(*this)
	{
		
	}


	void SColorWheel::Construct(const FArguments& in_args)
	{
		m_image = FCoreStyle::get().get_brush("ColorWheel.HueValueCircle");
		m_selector_image = FCoreStyle::get().get_brush("ColorWheel.Selector");

		m_on_value_changed = in_args._OnValueChanged;
	}

	FReply SColorWheel::On_Mouse_Button_On_Down(const FGeometry& my_geometry, const FPointerEvent& mouse_event)
	{
		if (mouse_event.get_effecting_button() == EKeys::LeftMouseButton)
		{
			if (!process_mouse_action(my_geometry, mouse_event, false))
			{
				return FReply::un_handled();
			}

			return FReply::handled().capture_mouse(shared_from_this());
		}

		return FReply::un_handled();
	}

	FReply SColorWheel::On_Mouse_Button_On_Up(const FGeometry& my_geometry, const FPointerEvent& mouse_event)
	{
		if (mouse_event.get_effecting_button() == EKeys::LeftMouseButton && has_mouse_capture())
		{
			return FReply::handled().release_mouse_capture();
		}

		return FReply::un_handled();
	}

	FReply SColorWheel::On_Mouse_Move(const FGeometry& my_geometry, const FPointerEvent& mouse_event)
	{
		if (!has_mouse_capture())
		{
			return FReply::un_handled();
		}

		process_mouse_action(my_geometry, mouse_event, true);

		return FReply::handled();
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
			in_widget_style.get_color_and_opacity_tint() * m_image->get_tint(in_widget_style));//note:widget style will be affected by FWidgetStyle

		//todo:implement calc relative position from center
		FSlateDrawElement::MakeBox(out_draw_elements,
			layer_id + 1,
			allotted_geometry.to_paint_geometry(0.5f * (allotted_geometry.get_local_size() + calc_relative_position_from_center() * circle_size - selector_size), selector_size),
			m_selector_image,
			draw_effects,
			in_widget_style.get_color_and_opacity_tint() * m_selector_image->get_tint(in_widget_style));

		return layer_id + 1;
	}

	glm::vec2 SColorWheel::Compute_Desired_Size(float) const
	{
		return glm::vec2();
	}

	glm::vec2 SColorWheel::calc_relative_position_from_center() const
	{
		float hue = m_selected_color.Get().r;
		float saturation = m_selected_color.Get().g;
		float angle = hue / 180.0f * 3.1415926535897932f;
		float radius = saturation;

		return glm::vec2(std::cos(angle), std::sin(angle)) * radius;
	}

	bool SColorWheel::process_mouse_action(const FGeometry& my_geometry, const FPointerEvent& mouse_event, bool b_process_when_outside_color_wheel)
	{
		const glm::vec2 local_mouse_coordinate = my_geometry.absolute_to_local(mouse_event.get_screen_space_position());

		const glm::vec2 relative_position_from_center = (2.0f * local_mouse_coordinate - my_geometry.get_local_size()) / (my_geometry.get_local_size() - m_selector_image->m_image_size);

		const float relative_radius = glm::sqrt(relative_position_from_center.x * relative_position_from_center.x + relative_position_from_center.y * relative_position_from_center.y);

		if (relative_radius <= 1.0f || b_process_when_outside_color_wheel)
		{
			float angle = std::atan2(relative_position_from_center.y, relative_position_from_center.x);

			if (angle < 0.0f)
			{
				angle += 2.0f * 3.1415926535897932f;//todo:add PI
			}

			m_selected_color.update_now(*this);
			glm::vec4 new_color = m_selected_color.Get();

			{
				new_color.r = angle * 180.0f * 0.31830988618f;//this is inv_pi
				new_color.g = std::min(relative_radius, 1.0f);
			}

			m_selected_color.Set(*this, new_color);//todo:remove this function

			//todo:implement on value changed	
			m_on_value_changed.execute_if_bound(new_color);
		}

		return (relative_radius <= 1.0f);
	}

}