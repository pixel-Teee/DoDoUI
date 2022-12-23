#include <PreCompileHeader.h>

#include "SWidget.h"

#include "SlateCore/Types/ISlateMetaData.h"

#include "SlateCore/Types/SlateAttributeDescriptor.h"

namespace DoDo {
	void SWidget::Private_Register_Attributes(FSlateAttributeInitializer& attribute_initializer)
	{
		//visibility should be the first attribute in the list
		//the order in which slate attribute are declared in the .h dictates of the order
		SLATE_ADD_MEMBER_ATTRIBUTE_DEFINITION_WITH_NAME(attribute_initializer, "Visibility", , EInvalidateWidgetReason::Visibility);
	}

}

