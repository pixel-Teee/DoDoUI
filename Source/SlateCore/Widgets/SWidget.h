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

		friend class FSlateAttributeMetaData;

		//SWidget();

	public:
		virtual ~SWidget();

	public:
		//layout

		/* return true if the widgets will update its registered slate attributes automatically or they need to be updated manually */
		bool Is_Attributes_Updates_Enabled() const { return m_b_enabled_attributes_update; }

	public:
		/*
		 * invalidates the widget from the view of a layout caching widget that may own this widget
		 * will force the owning widget to redraw and cache children on the next paint pass
		 */
		void Invalidate(EInvalidateWidgetReason in_validate_reason);

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

	protected:
		/*
		 * hidden default constructor
		 *
		 * use SNew(WidgetClassName) to instantiate new widgets
		 *
		 * @see SNew
		 */
		SWidget();

		/* is the widget construction completed(did we called and returned from the Construct() function) */
		bool Is_Constructed() const { return m_b_Is_Declarative_Syntax_Construction_Completed; }

	private:
		/* are bound slate attributes will be updated once per frame */
		uint8_t m_b_enabled_attributes_update : 1;

		/* the SNew or SAssignedNew construction is completed */
		uint8_t m_b_Is_Declarative_Syntax_Construction_Completed : 1;

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