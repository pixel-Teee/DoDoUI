#pragma once

#include "SlateCore/Widgets/InvalidateWidgetReason.h"

#include "SlateCore/Widgets/SWidget.h"//TPaddingSingleWidgetSlotMixin depends on it

namespace DoDo
{
	/* mixin to add the padding functionality to a base slot that is also a single children */
	template<typename MixedIntoType, EInvalidateWidgetReason InPaddingInvalidationReason = EInvalidateWidgetReason::Layout>
	class TPaddingSingleWidgetSlotMixin
	{
	public:
		template<typename WidgetType, typename V = typename std::enable_if<std::is_base_of<SWidget, WidgetType>::value>::type>
		TPaddingSingleWidgetSlotMixin(WidgetType& in_parent)
		{
			
		}
	protected:
		
	};
}