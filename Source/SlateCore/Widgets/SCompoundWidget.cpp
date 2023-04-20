#include <PreCompileHeader.h>
#include "SCompoundWidget.h"

#include "SlateCore/Layout/FlowDirection.h"//On_Arrange_Children depends on it
#include "SlateCore/Layout/LayoutUtils.h"//On_Arrange_Children depends on it
#include "SlateCore/Styling/WidgetStyle.h"

namespace DoDo {
	/*
	SCompoundWidget::SCompoundWidget()
	{
	}
	*/
	SLATE_IMPLEMENT_WIDGET(SCompoundWidget)
	void SCompoundWidget::Private_Register_Attributes(FSlateAttributeInitializer& attribute_initializer)
	{
		SLATE_ADD_MEMBER_ATTRIBUTE_DEFINITION_WITH_NAME(attribute_initializer, "SlotPadding", m_child_slot.m_slot_padding_attribute, EInvalidateWidgetReason::Layout);
		SLATE_ADD_MEMBER_ATTRIBUTE_DEFINITION_WITH_NAME(attribute_initializer, "ContentScale", m_content_scale_attribute, EInvalidateWidgetReason::Layout);
		SLATE_ADD_MEMBER_ATTRIBUTE_DEFINITION_WITH_NAME(attribute_initializer, "ColorAndOpacity", m_content_scale_attribute, EInvalidateWidgetReason::Paint);
		SLATE_ADD_MEMBER_ATTRIBUTE_DEFINITION_WITH_NAME(attribute_initializer, "ForegroundColor", m_foreground_color_attribute, EInvalidateWidgetReason::Paint);
	}


	SCompoundWidget::~SCompoundWidget()
	{
	}

	int32_t SCompoundWidget::On_Paint(const FPaintArgs& args, const FGeometry& allotted_geometry,
		const FSlateRect& my_culling_rect, FSlateWindowElementList& out_draw_elements, int32_t layer_id,
		const FWidgetStyle& in_widget_style, bool b_parent_enabled) const
	{
		//SButton's OnPaint will call this function

		//todo:implement FArrangedChildren
		//parameter is filter, just visible widget can add to FArrangedChildren
		FArrangedChildren arranged_children(EVisibility::visible);
		{
			this->Arrange_Children(allotted_geometry, arranged_children);
		}

		//there may be zero elements in this array if your child collapsed/hidden
		if(arranged_children.num() > 0)
		{
			//todo:implement should be enabled function
			const bool b_should_be_enabled = false;

			FArrangedWidget& the_child = arranged_children[0];

			//todo:implement FWidgetStyle
			FWidgetStyle compounded_widget_style = FWidgetStyle(in_widget_style)
				.blend_color_and_opacity_tint(get_color_and_opacity())
				.set_foreground_color(b_should_be_enabled ? get_foreground_color() : get_disabled_foreground_color());

			int32_t layer = 0;

			//todo:implement SWidget's paint function
			layer = the_child.m_widget->paint(args, the_child.m_geometry, my_culling_rect, out_draw_elements, layer_id + 1, compounded_widget_style, b_should_be_enabled);

			return layer;
		}

		return layer_id;
	}

	FChildren* SCompoundWidget::Get_Children()
	{
		//just to return FChildren(FChildren may own slot, slot owns SWidget)
		return &m_child_slot;
	}

	FSlateColor SCompoundWidget::get_foreground_color() const
	{
		return m_foreground_color_attribute.Get();
	}

	void SCompoundWidget::On_Arrange_Children(const FGeometry& allotted_geometry,
	                                          FArrangedChildren& arranged_children) const
	{
		if(m_child_slot.get_widget() != nullptr)//todo:remove this
		{
			//call template function
			Arrange_Single_Child(g_slate_flow_direction, allotted_geometry, arranged_children, m_child_slot, Get_Content_Scale());
		}
	}

	SCompoundWidget::SCompoundWidget()
		: m_child_slot(this)
		, m_content_scale_attribute(*this, glm::vec2(1.0f, 1.0f))
		, m_color_and_opacity_attribute(*this, FLinearColor::White)//white
		, m_foreground_color_attribute(*this, FSlateColor::use_foreground())//todo:use foreground
	{
	}

	glm::vec2 SCompoundWidget::Compute_Desired_Size(float) const
	{

		std::shared_ptr<SWidget> widget = m_child_slot.get_widget();

		if (widget != nullptr)
		{
			EVisibility child_visibility = m_child_slot.get_widget()->get_visibility();

			if (child_visibility != EVisibility::Collapsed)
			{
				//widget's size + FMargin's size(padding)
				//from slot's content to get the widget
				return m_child_slot.get_widget()->get_desired_size() + m_child_slot.get_padding().get_desired_size();
			}
		}

		return glm::vec2(0.0f);
	}
}
