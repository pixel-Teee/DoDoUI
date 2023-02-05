#pragma once

#include "Slate/Widgets/Layout/SBorder.h"

#include "SlateCore/Styling/CoreStyle.h"
#include "SlateCore/Styling/ISlateStyle.h"
//#include "SlateCore/Styling/SlateStyle.h"
#include "SlateCore/Styling/SlateTypes.h"

namespace DoDo
{
	struct FButtonStyle;
	struct FKeyEvent;

	class SButton : public SBorder
	{
		SLATE_DECLARE_WIDGET(SButton, SBorder)
	public:

		SLATE_BEGIN_ARGS(SButton)
		: _Content()
		, _ButtonStyle(&FCoreStyle::get().get_widget_style<FButtonStyle>("Button"))//todo:implement FCoreStyle1
		, _HAlign(HAlign_Fill)
		, _VAlign(VAlign_Fill)
		, _ContentPadding(FMargin(4.0f, 2.0f))
		, _DesiredSizeScale(glm::vec2(1.0f, 1.0f))
		, _ContentScale(glm::vec2(1.0f, 1.0f))
		, _ButtonColorAndOpacity(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f))//white
		, _ForegroundColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f))
		{
			
		}
		/*slot for this button's content(optional)*/
		SLATE_DEFAULT_SLOT(FArguments, Content)

		/*the visual style of the button*/
		SLATE_STYLE_ARGUMENT(FButtonStyle, ButtonStyle)

		/*horizontal alignment*/
		SLATE_ARGUMENT(EHorizontalAlignment, HAlign)

		/*vertical alignment*/
		SLATE_ARGUMENT(EVerticalAlignment, VAlign)

		/*spacing between button's border and the content*/
		SLATE_ATTRIBUTE(FMargin, ContentPadding)

		/*called when the button is clicked*/
		//todo:implement SLATE_EVENT

		SLATE_ATTRIBUTE(glm::vec2, DesiredSizeScale)

		SLATE_ATTRIBUTE(glm::vec2, ContentScale)

		SLATE_ATTRIBUTE(glm::vec4, ButtonColorAndOpacity)//note:this control's content color

		SLATE_ATTRIBUTE(glm::vec4, ForegroundColor)

		SLATE_END_ARGS()

	protected:
		SButton();

	public:
		/*
		 * returns true if this button is currently pressed
		 *
		 * @return true if pressed, otherwise false
		 * @note IsPressed used to be virtual, use SetAppearPressed to assign attribute if you need to override the default behaivor
		 */
		bool is_pressed() const
		{
			return m_b_is_pressed || m_appear_pressed_attribute.Get();
		}
	public:

		/*
		 * construct this widget
		 *
		 * @param InArgs the declaration data for this widget
		 */
		void Construct(const FArguments& in_args);

		/*see content padding attribute*/
		void set_content_padding(TAttribute<FMargin> in_content_padding);

	public:
		virtual int32_t On_Paint(const FPaintArgs& args, const FGeometry& allotted_geometry, const FSlateRect& my_culling_rect, FSlateWindowElementList& out_draw_elements, int32_t layer_id, const FWidgetStyle& in_widget_style, bool b_parent_enabled) const override;

		virtual FReply On_Key_Down(const FGeometry& my_geometry, const FKeyEvent& in_key_event) override;

	protected:
		/*press the button*/
		virtual void Press();

		/*@return combines the user-specified margin and the button's internal margin*/
		FMargin get_combined_padding() const;

	private:
		void update_press_state_changed();

		void update_padding();

		void update_border_image();

		void update_foreground_color();

	private:
		/*the location in screen space the button was pressed*/
		glm::vec2 m_pressed_screen_space_position;

		/*style resource for the button*/
		//todo:implement FButtonStyle
		const FButtonStyle* m_style;
		/*the delegate to execute when button is clicked*/
		//todo:implement FReply
		FSimpleDelegate m_on_clicked;

		/*the delegate to execute when button is pressed*/
		//todo:implement FSlateDelegate
		FSimpleDelegate m_on_pressed;

		/*the delegate to execute when button is released*/
		//todo:implement
		FSimpleDelegate m_on_released;

		/*the delegate to execute when button is hovered*/
		//todo:implement
		FSimpleDelegate m_on_hovered;

		/*the delegate to execute when the button exit the hovered state*/
		//todo:implement
		FSimpleDelegate m_on_unhovered;

		/*the sound to play when the button is hovered*/
		//todo:implement FSlateSound

		/*sets whether a click should be triggered on mouse down, mouse up, or that both a mouse down and up are required*/
		//todo:implement ClickMethod

		//todo:implement TouchMethod

		//todo:implement PressMethod

		/*can this button be focused?*/
		uint8_t m_b_is_focusable : 1;

		/*true if this button is currently in a pressed state*/
		uint8_t m_b_is_pressed : 1;

	private:
		/*other color information*/
		//todo:implement

		/*optional foreground color that will be inherited by all of this widget's contents*/
		TSlateAttribute<glm::vec4> m_border_foreground_color_attribute;

		/*padding specified by the user, it will be combind with the button's internal padding*/
		TSlateAttribute<FMargin> m_content_padding_attribute;//control content's padding

		/*optional foreground color that will be inherited by all of this widget's contents*/
		TSlateAttribute<bool> m_appear_pressed_attribute;
	};
}