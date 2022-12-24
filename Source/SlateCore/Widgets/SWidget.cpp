#include <PreCompileHeader.h>

#include "SWidget.h"

#include "SlateCore/Types/ISlateMetaData.h"

#include "SlateCore/Types/SlateAttributeDescriptor.h"

namespace DoDo {
	//this function will be called at FSlateWidgetClassData construct
	void SWidget::Private_Register_Attributes(FSlateAttributeInitializer& attribute_initializer)
	{
		//just to register some static information of slate attribute
		//visibility should be the first attribute in the list
		//the order in which slate attribute are declared in the .h dictates of the order
		//attribute name
		//calculate SWidget and m_Visibility_Attribute offset
		//FInvalidateWidgetReasonAttribute(may be with callback)
		//attribute_initializer.add_member_attribute("Visibility", STRUCT_OFFSET(PrivateThisType, m_Visibility_Attribute), FSlateAttributeDescriptor::FInvalidateWidgetReasonAttribute{EInvalidateWidgetReason::Visibility}).affect_visibility();
		SLATE_ADD_MEMBER_ATTRIBUTE_DEFINITION_WITH_NAME(attribute_initializer, "Visibility", m_Visibility_Attribute, EInvalidateWidgetReason::Visibility).affect_visibility();

	}

	SWidget::~SWidget()
	{
	}
}

