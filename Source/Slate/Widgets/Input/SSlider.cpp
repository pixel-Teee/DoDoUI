#include "PreCompileHeader.h"

#include "SSlider.h"

#include "SlateCore/Rendering/RenderingCommon.h"//ESlateDrawEffect

namespace DoDo {
	SSlider::SSlider()
	{

	}
	void SSlider::Construct(const SSlider::FArguments& in_declaration)
	{
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
		const glm::vec2 handle_size = glm::vec2(20.0f, 20.0f);//todo:implement get thumb image size
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



		//todo:draw
		return layer_id;
	}
}
