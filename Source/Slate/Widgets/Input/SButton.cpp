#include <PreCompileHeader.h>

#include "SButton.h"

#include "SlateCore/Layout/Geometry.h"
#include "SlateCore/Rendering/DrawElements.h"//FSlateDrawElement depends on it

//#include "SlateCore/Styling/SlateTypes.h"

namespace DoDo
{
	SButton::SButton()
		: m_border_foreground_color_attribute(*this)
		, m_content_padding_attribute(*this)
		, m_appear_pressed_attribute(*this)
	{
	}

	void SButton::Private_Register_Attributes(FSlateAttributeInitializer& attribute_initializer)
	{
		//SLATE_ADD_MEMBER_ATTRIBUTE_DEFINITION_WITH_NAME(attribute_initializer, "SlotPadding", m_child_slot.m_slot_padding_attribute, EInvalidateWidgetReason::Layout);
		//SLATE_ADD_MEMBER_ATTRIBUTE_DEFINITION_WITH_NAME(attribute_initializer, "ContentScale", m_content_scale_attribute, EInvalidateWidgetReason::Layout);
		//SLATE_ADD_MEMBER_ATTRIBUTE_DEFINITION_WITH_NAME(attribute_initializer, "ColorAndOpacity", m_content_scale_attribute, EInvalidateWidgetReason::Paint);
		//SLATE_ADD_MEMBER_ATTRIBUTE_DEFINITION_WITH_NAME(attribute_initializer, "ForegroundColor", m_foreground_color_attribute, EInvalidateWidgetReason::Paint);
	}

	void SButton::Construct(const FArguments& in_args)
	{
		m_border_foreground_color_attribute.Assign(*this, in_args._ForegroundColor);

		m_on_pressed = in_args._OnPressed;

		SBorder::Construct(SBorder::FArguments()
			.ContentScale(in_args._ContentScale)
			.DesiredSizeScale(in_args._DesiredSizeScale)
			.BorderBackgroundColor(in_args._ButtonColorAndOpacity)
			.HAlign(in_args._HAlign)
			.VAlign(in_args._VAlign)
		);//todo:implement determine Content

		//todo:implement set button style
		set_button_style(in_args._ButtonStyle);
	}

	void SButton::set_content_padding(TAttribute<FMargin> in_content_padding)
	{
		m_content_padding_attribute.Assign(*this, std::move(in_content_padding));
	}

	//void SButton::set_on_pressed(FSimpleDelegate in_on_pressed)
	//{
	//	m_on_pressed = in_on_pressed;
	//}

	void SButton::set_button_style(const FButtonStyle* button_style)
	{
		m_style = button_style;

		update_padding();
		update_border_image();
		update_foreground_color();
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

		bool b_enabled = true;

		const FSlateBrush* brush_resource = get_border_image();

		if(brush_resource && brush_resource->m_draw_as != ESlateBrushDrawType::NoDrawType)
		{
			FSlateDrawElement::MakeBox(out_draw_elements,
				layer_id,
				allotted_geometry.to_paint_geometry(),
				brush_resource,
				ESlateDrawEffect::None,
				brush_resource->get_tint() * get_border_back_ground_color());
		}

		return SCompoundWidget::On_Paint(args, allotted_geometry, my_culling_rect, out_draw_elements, layer_id, in_widget_style, b_parent_enabled);
	}

	FReply SButton::On_Key_Down(const FGeometry& my_geometry, const FKeyEvent& in_key_event)
	{
		FReply reply = FReply::un_handled();
		if(Is_Enabled())
		{
			//todo:implement Press
			Press();
		}
		else
		{
			reply = SBorder::On_Key_Down(my_geometry, in_key_event);//call SWidget's On_Key_Down
		}

		//return the constructed reply
		return reply;
	}

	FReply SButton::On_Mouse_Button_On_Down(const FGeometry& my_geometry, const FPointerEvent& mouse_event)
	{
		FReply reply = FReply::un_handled();

		if(Is_Enabled()) //todo:check is left mouse button
		{
			Press();

			//todo:get the reply from execute function
			reply = FReply::handled();
		}

		return reply;
	}

	FReply SButton::On_Mouse_Button_On_Up(const FGeometry& my_geometry, const FPointerEvent& mouse_event)
	{
		FReply reply = FReply::un_handled();

		Release();

		if(Is_Enabled())
		{
			//todo:implement this
		}

		return reply;
	}

	void SButton::Press()
	{
		if(!m_b_is_pressed)
		{
			m_b_is_pressed = true;

			//todo:implement play pressed source

			//todo:call delegate
			m_on_pressed.execute_if_bound();

			//todo:implement update press state changed
			update_press_state_changed();//update button state, image color something
		}
	}

	void SButton::Release()
	{
		if(m_b_is_pressed)
		{
			m_b_is_pressed = false;
			//todo:implement on released
			update_press_state_changed();
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

	SButton::~SButton()
	{
	}
}
