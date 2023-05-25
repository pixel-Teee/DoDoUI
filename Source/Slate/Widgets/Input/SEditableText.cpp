#include "PreCompileHeader.h"

#include "SEditableText.h"

namespace DoDo {
	SEditableText::SEditableText()
	{
	}
	SEditableText::~SEditableText()
	{
	}
	void SEditableText::Construct(const FArguments& in_args)
	{
		m_plain_text_marshaller = FPlainTextLayoutMarshaller::Create();

		m_editable_text_layout = std::make_unique<FSlateEditableTextLayout>(*this, in_args._Text, *(in_args._Style), in_args._TextShapingMethod, in_args._TextFlowDirection, m_plain_text_marshaller);
	}
	int32_t SEditableText::On_Paint(const FPaintArgs& args, const FGeometry& allotted_geometry, const FSlateRect& my_culling_rect, FSlateWindowElementList& out_draw_elements, int32_t layer_id, const FWidgetStyle& in_widget_style, bool b_parent_enabled) const
	{
		const FTextBlockStyle& editable_text_style = m_editable_text_layout->get_text_style();
		const FLinearColor foreground_color = editable_text_style.m_color_and_opacity.get_color(in_widget_style);

		FWidgetStyle text_widget_style = FWidgetStyle(in_widget_style)
			.set_foreground_color(foreground_color);

		layer_id = m_editable_text_layout->On_Paint(args, allotted_geometry, my_culling_rect, out_draw_elements, layer_id, text_widget_style, should_be_enabled(b_parent_enabled));

		return layer_id;
	}
	FReply SEditableText::On_Key_Char(const FGeometry& my_geometry, const FCharacterEvent& in_character_event)
	{
		FReply reply = FReply::un_handled();

		//first call the user defined key handler, there might be overrides to normal functionality

		if (!reply.is_event_handled())
		{
			reply = m_editable_text_layout->handle_key_char(in_character_event);
		}

		return reply;
	}
	FReply SEditableText::On_Key_Down(const FGeometry& my_geometry, const FKeyEvent& in_key_event)
	{
		//FKey key = in_key_event.get_key();

		//todo:add more logic

		return FReply::un_handled();
	}
	glm::vec2 SEditableText::Compute_Desired_Size(float Layout_Scale_Multiplier) const
	{
		glm::vec2 text_layout_size = m_editable_text_layout->Compute_Desired_Size(Layout_Scale_Multiplier);

		return text_layout_size;
	}
	FChildren* SEditableText::Get_Children()
	{
		return m_editable_text_layout->Get_Children();
	}
	void SEditableText::On_Arrange_Children(const FGeometry& allotted_geometry, FArrangedChildren& arranged_children) const
	{
	}
	std::shared_ptr<SWidget> SEditableText::get_slate_widget()
	{
		return shared_from_this();
	}
}