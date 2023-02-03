#include <PreCompileHeader.h>

#include "SButton.h"

#include "SlateCore/Styling/SlateTypes.h"

namespace DoDo
{
	SButton::SButton()
		: m_border_foreground_color_attribute(*this)
		, m_content_padding_attribute(*this)
		, m_appear_pressed_attribute(*this)
	{
	}

	void SButton::Construct(const FArguments& in_args)
	{
		m_border_foreground_color_attribute.Assign(*this, in_args._ForegroundColor);

		SBorder::Construct(SBorder::FArguments()
			.ContentScale(in_args._ContentScale)
			.DesiredSizeScale(in_args._DesiredSizeScale)
			.BorderBackgroundColor(in_args._ButtonColorAndOpacity)
			.HAlign(in_args._HAlign)
			.VAlign(in_args._VAlign)
		);//todo:implement determine Content

		
	}

	void SButton::set_content_padding(TAttribute<FMargin> in_content_padding)
	{
		m_content_padding_attribute.Assign(*this, std::move(in_content_padding));
	}

	int32_t SButton::On_Paint(const FPaintArgs& args, const FGeometry& allotted_geometry,
	                          const FSlateRect& my_culling_rect, FSlateWindowElementList& out_draw_elements, int32_t layer_id,
	                          const FWidgetStyle& in_widget_style, bool b_parent_enabled) const
	{
		//return SBorder::On_Paint(args, allotted_geometry, my_culling_rect, out_draw_elements, layer_id, in_widget_style,
		//                         b_parent_enabled);

		//todo:implement should be enabled

		//todo:implement get show disabled effect

		//todo:implement get border image

		//todo:implement ESlateDrawEffect

		//todo:implement FSlateDrawElement::MakeBox
		return SCompoundWidget::On_Paint(args, allotted_geometry, my_culling_rect, out_draw_elements, layer_id, in_widget_style, b_parent_enabled);
	}

	FReply SButton::On_Key_Down(const FGeometry& my_geometry, const FKeyEvent& in_key_event)
	{
		FReply reply = FReply::un_handled();
		if(Is_Enabled())
		{
			//todo:implement Press
		}
		else
		{
			reply = SBorder::On_Key_Down(my_geometry, in_key_event);//call SWidget's On_Key_Down
		}

		//return the constructed reply
		return reply;
	}

	void SButton::Press()
	{
		if(!m_b_is_pressed)
		{
			m_b_is_pressed = true;

			//todo:implement play pressed source

			//todo:call delegate
			m_on_pressed.Invoke();

			//todo:implement update press state changed
			update_press_state_changed();//update button state, image color something
		}
	}

	FMargin SButton::get_combined_padding() const
	{
		return (is_pressed())
			? m_content_padding_attribute.Get() + m_style->m_pressed_padding
			: m_content_padding_attribute.Get() + m_style->m_normal_padding;
	}

	void SButton::update_press_state_changed()
	{
		update_padding();

		update_border_image();

		update_foreground_color();
	}

	void SButton::update_padding()
	{
		//it is some narrow action
		set_padding(get_combined_padding());
	}

	void SButton::update_border_image()
	{
		if(is_pressed())
		{
			set_border_image(&m_style->m_pressed);
		}
		else if(is_hovered())
		{
			set_border_image(&m_style->m_hovered);
		}
		else
		{
			set_border_image(&m_style->m_normal);
		}
	}

	void SButton::update_foreground_color()
	{
		//todo:check border foreground color attribute use FWidgetStyle color

		set_foreground_color(m_border_foreground_color_attribute.Get());
	}
}
