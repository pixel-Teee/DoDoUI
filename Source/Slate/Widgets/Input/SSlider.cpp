#include "PreCompileHeader.h"

#include "SSlider.h"

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
	int32_t SSlider::On_Paint(const FPaintArgs& args, const FGeometry& allotted_geometry, const FSlateRect& my_culling_rect, FSlateWindowElementList& out_draw_elements, int32_t layer_id, const FWidgetStyle& in_widget_style, bool b_parent_enabled) const
	{
		//todo:draw
		return layer_id;
	}
}