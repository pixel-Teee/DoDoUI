#pragma once

#include "SlateCore/Widgets/SLeafWidget.h"

#include "SlateCore/Types/SlateEnums.h"//EOrientation

#include "ApplicationCore/GenericPlatform/ICursor.h"//EMouseCursor depends on it

#include "SlateCore/Widgets/DeclarativeSyntaxSupport.h"

#include "SlateCore/Styling/SlateTypes.h"//FSliderStyle depends on it

#include "SlateCore/Styling/CoreStyle.h"//FCoreStyle depends on it

#include "SlateCore/Styling/ISlateStyle.h"//ISlateStyle depends on it

namespace DoDo {
	/*
	* a slate slider control is a linear scale and draggable handle
	*/
	class SSlider : public SLeafWidget
	{
	public:
		SLATE_BEGIN_ARGS(SSlider)
			: _IndentHandle(true)
			, _MouseUsesStep(false)
			, _RequiresControllerLock(true)
			, _Locked(false)
			, _Orientation(EOrientation::Orient_Horizontal)
			, _SliderBarColor(glm::vec4(1.0f))
			, _SliderHandleColor(glm::vec4(1.0f))
			, _Style(&FCoreStyle::get().get_widget_style<FSliderStyle>("Slider"))
			, _StepSize(0.01f)
			, _Value(1.0f)
			, _MinValue(0.0f)
			, _MaxValue(1.0f)
			, _IsFocusable(true)
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
			SLATE_STYLE_ARGUMENT(FSliderStyle, Style)

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

		SSlider();

		/*
		* construct the widget
		* 
		* @param InDeclaration a declaration from which to construct the widget
		*/
		void Construct(const SSlider::FArguments& in_declaration);

		/*
		 * get the value attribute scaled from 0 to 1
		 */
		float get_normalized_value() const;

	public:
		//SWidget overrides

		virtual int32_t On_Paint(const FPaintArgs& args, const FGeometry& allotted_geometry,
			const FSlateRect& my_culling_rect, FSlateWindowElementList& out_draw_elements, int32_t layer_id, const FWidgetStyle& in_widget_style, bool b_parent_enabled) const override;

		virtual glm::vec2 Compute_Desired_Size(float) const;

	protected:

		const FSlateBrush* get_bar_image() const;

		const FSlateBrush* get_thumb_image() const;
		
	protected:
		//todo:implement FSliderStyle
		//holds the style passed to the widget upon construction
		const FSliderStyle* m_style;

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