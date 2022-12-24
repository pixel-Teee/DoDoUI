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
}
