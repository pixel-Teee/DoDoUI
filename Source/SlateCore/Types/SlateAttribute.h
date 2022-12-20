#pragma once

#include <Core/String/DoDoString.h>//todo:put to there, SlateAttributeDefinition.inl depends on this header file
#include "Core/Delegates/IDelegateInstance.h"//SlateAttributeDefinition.inl depends on this header file
#include "SlateCore/Widgets/InvalidateWidgetReason.h"
#include "Core/Misc/Attribute.h"//SlateAttributeBase.inl depends on this header file

namespace DoDo {
	class SWidget;
	/*
		base struct of all slate attribute type
	*/
	struct FSlateAttributeBase
	{
		/*
			not all invalidation is supported by slate attribute
			child order : the update of slate attribute is done in the slate prepass, we can't add or remove children in slate prepass
			attribute registration : in fast path, the slate attribute are updated in a loop, the iterator can't be modified while we are looping
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

	//default predicate to compare of object for slate attribute
	template<typename ComparePredicate = std::equal_to<>>
	struct TSlateAttributeComparePredicate
	{
		template<typename ObjectType>
		static bool Identical_To(const SWidget&, ObjectType&& lhs, ObjectType&& rhs)
		{
			//construct a ComparePredicate object, and call it's operator, to compare two objects
			return ComparePredicate{}(std::forward<ObjectType>(lhs), std::forward<ObjectType>(rhs));
		}
	};

	//predicate that returns the InvalidationReason defined as argument type
	template<EInvalidateWidgetReason InvalidationReason>
	struct TSlateAttributeInvalidationReason
	{
		static_assert(FSlateAttributeBase::Is_Invalidate_Widget_Reason_Supported(InvalidationReason));
		static constexpr EInvalidateWidgetReason Get_Invalidation_Reason(const SWidget&) { return InvalidationReason; }
	};

	#include "SlateAttributeDefinition.inl"
	#include "SlateAttributeBase.inl"

	#include "SlateAttributeContained.inl"
	#include "SlateAttributeManaged.inl"
	#include "SlateAttributeMember.inl"
}