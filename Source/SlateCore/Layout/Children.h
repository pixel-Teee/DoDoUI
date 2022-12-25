#pragma once

#include "ChildrenBase.h"//FChildren depends on it

#include "SlateCore/SlotBase.h"//TSlotBase depends on it

#include "SlateCore/Widgets/InvalidateWidgetReason.h"

namespace DoDo
{
	/* a FChildren that has only one child and can take a templated slot */
	template<typename SlotType>
	class TSingleWidgetChildrenWithSlot : public FChildren
	{
		
	};

	/* a FChildren that has only one child and support alignment and padding */
	template<EInvalidateWidgetReason In_Padding_Invalidation_Reason = EInvalidateWidgetReason::Layout>
	class TSingleWidgetChildrenWithBasicLayoutSlot
	{
		
	};
}
