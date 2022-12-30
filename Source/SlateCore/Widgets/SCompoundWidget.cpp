#include <PreCompileHeader.h>
#include "SCompoundWidget.h"

namespace DoDo {
	/*
	SCompoundWidget::SCompoundWidget()
	{
	}
	*/
	void SCompoundWidget::Private_Register_Attributes(FSlateAttributeInitializer& attribute_initializer)
	{
		SLATE_ADD_MEMBER_ATTRIBUTE_DEFINITION_WITH_NAME(attribute_initializer, "ContentScale", m_content_scale_attribute, EInvalidateWidgetReason::Layout);
		SLATE_ADD_MEMBER_ATTRIBUTE_DEFINITION_WITH_NAME(attribute_initializer, "ColorAndOpacity", m_content_scale_attribute, EInvalidateWidgetReason::Paint);
		SLATE_ADD_MEMBER_ATTRIBUTE_DEFINITION_WITH_NAME(attribute_initializer, "ForegroundColor", m_foreground_color_attribute, EInvalidateWidgetReason::Paint);
	}


	SCompoundWidget::~SCompoundWidget()
	{
	}

	glm::vec2 SCompoundWidget::Compute_Desired_Size(float) const
	{
		EVisibility child_visibility = m_child_slot.get_widget()->get_visibility();

		if(child_visibility != EVisibility::Collapsed)
		{
			//widget's size + FMargin's size(padding)
			//from slot's content to get the widget
			return m_child_slot.get_widget()->get_desired_size() + m_child_slot.get_padding().get_desired_size();
		}

		return glm::vec2(0.0f);
	}
}
