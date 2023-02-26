#pragma once

#include "SlateCore/Widgets/SLeafWidget.h"

#include "SlateCore/Types/SlateEnums.h"//EOrientation

#include "ApplicationCore/GenericPlatform/ICursor.h"//EMouseCursor depends on it

#include "SlateCore/Widgets/DeclarativeSyntaxSupport.h"

namespace DoDo {
	/*
	* a slate slider control is a linear scale and draggable handle
	*/
	class SSlider : public SLeafWidget
	{
	public:
		SLATE_BEGIN_ARGS(SSlider)
			: _IndentHandle(true)
			{}

			/*whether the slidable area should be indented to fit the handle*/
			SLATE_ATTRIBUTE(bool, IndentHandle)

			/*sets new value if mouse position is greater/less than half the step size*/
			SLATE_ARGUMENT(bool, MouseUsesStep)

			/*sets whether we have to lock input to change the slider value*/
			SLATE_ARGUMENT(bool, RequiresControllerLock)

			/*whether the handle is interactive or fixed*/
			SLATE_ATTRIBUTE(bool, Locked)

			/*the slider's orientation*/
			SLATE_ARGUMENT(EOrientation, Orientation)

			/*the color to draw the slider bar in*/
			SLATE_ATTRIBUTE(glm::vec4, SliderBarColor)

			/*the color to draw the slider handle in*/
			SLATE_ATTRIBUTE(glm::vec4, SliderHandleColor)

			/*the style used to draw the slider*/

			/*the input mode while using the controller*/
			SLATE_ATTRIBUTE(float, StepSize)

			/*a value that drives where the slider handle appears, value is normalized between 0 and 1*/
			SLATE_ATTRIBUTE(float, Value)

			/*the minimum value that can be specified by using the slider*/
			SLATE_ARGUMENT(float, MinValue)

			/*the maximum value that can be specified by using the slider*/
			SLATE_ARGUMENT(float, MaxValue)

			/*sometimes a slider should only be mouse-clickable and never keyboard focusable*/
			SLATE_ARGUMENT(bool, IsFocusable)

			//todo:add event
		SLATE_END_ARGS()
	protected:
		//todo:implement FSliderStyle

		//holds a flag indicating whether the slideable area should be indeted to fit the handle
		TAttribute<bool> m_indent_handle;

		//holds a flag indicating whether the slider is locked
		TAttribute<bool> m_locked_attribute;

		//hold's the slider's orientation
		EOrientation m_orientation;

		//holds the color of the slider bar
		TAttribute<glm::vec4> m_slider_bar_color;

		//holds the color of the slider handle
		TAttribute<glm::vec4>m_slider_handle_color;

		//holds the slider's current value
		TAttribute<float> m_value_attribute;

		//holds the initial cursor in case a custom cursor has been specified, so we can restore it after dragging the slider
		EMouseCursor::Type m_cached_cursor;

		/*the location in screenspace the slider was pressed by a touch*/
		glm::vec2 m_pressed_screen_space_touch_down_position = glm::vec2(0.0f, 0.0f);

		/*holds the amount to adjust the value by when using a controller or keyboard*/
		TAttribute<float> m_step_size;

		float m_min_value;

		float m_max_value;

		//holds a flag indicating whether a controller/keyboard is manipulating the slider's value
		//when true, navigation away from the widget is prevented until a new value has been accepted or canceled
		bool m_b_controller_input_captured;

		/*sets new value if mouse position is greater/less than half the step size*/
		bool m_b_mouse_uses_step;

		/*sets whether we have to lock input to change the slider value*/
		bool m_b_required_controller_lock;

		/*when true, this slider will be keyboard focusable, defaults to false*/
		bool m_b_is_focusable;

	private:

	};
}