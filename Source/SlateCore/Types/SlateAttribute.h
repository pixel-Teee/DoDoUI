#pragma once

#include "SlateCore/Widgets/InvalidateWidgetReason.h"

namespace DoDo {
	/*
		base struct of all slate attribute type
	*/
	struct FSlateAttributeBase
	{
		/*
			not all invalidation is supported by slate attribute
			child order : the update of slate attribute is done in the slate prepass, we can't add or remove children is slate prepass
			attribute registration : in fase path, the slate attribute are updated in a loop, the iterator can't be modified while we are looping
		*/
		template<typename T>
		constexpr static bool Is_Invalidate_Widget_Reason_Supported(T Reason)
		{
			return false;
		}

		constexpr static bool Is_Invalidate_Widget_Reason_Supported(EInvalidateWidgetReason Reason)
		{
			return (Reason & (EInvalidateWidgetReason::Child_Order | EInvalidateWidgetReason::Attribute_Registration)) == EInvalidateWidgetReason::None;
		}
	};

	#include "SlateAttributeDefinition.inl"
}