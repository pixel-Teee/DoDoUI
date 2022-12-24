#pragma once

#include <Core/Core.h>

#include <type_traits>

#include "SlateCore/Types/SlateAttribute.h"
#include "SlateCore/Widgets/SlateControlledConstruction.h"

#include "SlateCore/Layout/Visibility.h"//m_Visibility_Attribute depends on it(EVisibility)

namespace DoDo
{
	class ISlateMetaData;

	class SWidget : public FSlateControlledConstruction
	{
	public:
		//widgets should only ever be constructed via SNew or SAssignNew
		//todo:need to implement Private_Register Attributes static function
		SLATE_DECLARE_WIDGET(SWidget, FSlateControlledConstruction)

		//SWidget();

	public:
		virtual ~SWidget();

	protected:
		/*
		a slate attribute that is member variable of a SWidget
		usage : TSlateAttribute<int32_t> MyAttribute1; TSlateAttribute<int32_t, EInvalidateWidgetReason::Paint> MyAttribute2; TSlateAttribute<int32_t, EInvalidateWidgetReason::Paint, TSlateAttributeComparePredicate<>> MyAttribute3;
		*/
		template<typename InObjectType, EInvalidateWidgetReason InInvalidationReasonValue = EInvalidateWidgetReason::None, typename InComparePredicate = TSlateAttributeComparePredicate<>>
		struct TSlateAttribute : public SlateAttributePrivate::TSlateMemberAttribute<
			InObjectType,
			typename std::conditional<InInvalidationReasonValue == EInvalidateWidgetReason::None,
			SlateAttributePrivate::FSlateAttributeNoInvalidationReason,
			TSlateAttributeInvalidationReason<InInvalidationReasonValue>>::type,
			InComparePredicate>
		{
			using SlateAttributePrivate::TSlateMemberAttribute<
				InObjectType,
				typename std::conditional<InInvalidationReasonValue == EInvalidateWidgetReason::None,
				SlateAttributePrivate::FSlateAttributeNoInvalidationReason,
				TSlateAttributeInvalidationReason<InInvalidationReasonValue>>::type,
				InComparePredicate>::TSlateMemberAttribute;
		};

	public:
		//return true if the widgets has any bound slate attribute
		bool Has_Registered_Slate_Attribute() const { return m_b_has_registered_slate_attribute; }

		//todo:move this member to private control
		//meta data associated with this widget
		std::vector<std::shared_ptr<ISlateMetaData>> m_Meta_Data;
	private:

		TSlateAttribute<EVisibility> m_Visibility_Attribute;

		//is there at least one slate attribute currently registered
		uint8_t m_b_has_registered_slate_attribute : 1;
	};
}