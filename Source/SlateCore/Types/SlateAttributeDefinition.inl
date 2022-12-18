#pragma once

#include <Core/String/DoDoString.h>

class SWidget;

namespace SlateAttributePrivate {

	enum class ESlateAttributeType : uint8_t {
		Member = 0,//member of a swidget(are not allowed to move)
		Managed = 1,//external to the swidget, global variable or member that can moved
		Contained = 2//inside a FSloat or other container that use dynamic memory(always attached to one and only one swidget)
	};

	//interface for structure that can be used to contain a slate attribute instead of a swidget
	class ISlateAttributeContainer
	{
	public:
		virtual SWidget& Get_Container_Widget() const = 0;

		virtual DoDo::DoDoUtf8String Get_Container_Name() const = 0;

		virtual uint32_t Get_Container_Sort_Order() const = 0;

	protected:

		/* implement to SlateAttribute.cpp */
		void Remove_Container_Widget(SWidget& widget);

		void Update_Container_Sort_Order(SWidget& widget);
		/* implement to SlateAttribute.cpp */
	};

	class ISlateAttributeGetter
	{
	public:
		struct FUpdateAttributeResult
		{
			FUpdateAttributeResult(EInvalidateWidgetReason In_Invalidation_Reason)
				: m_Invalidation_Reason(In_Invalidation_Reason)
				, m_b_Invalidation_Requested(true)
			{}

			FUpdateAttributeResult()
				: m_Invalidation_Reason(EInvalidateWidgetReason::None)
				, m_b_Invalidation_Requested(false)
			{}

			EInvalidateWidgetReason m_Invalidation_Reason;
			bool m_b_Invalidation_Requested;
		};

		virtual FUpdateAttributeResult Update_Attribute(const SWidget& widget) = 0;

		virtual const FSlateAttributeBase& Get_Attribute() const = 0;

		virtual void Set_Attribute(FSlateAttributeBase&) = 0;

		virtual ~ISlateAttributeGetter() = default;
	};

	struct FSlateAttributeImpl : public FSlateAttributeBase
	{
	protected:
		void protected_unregister_attribute(SWidget& widget, ESlateAttributeType attribute_type) const;

		void protected_register_attribute(SWidget& widget, ESlateAttributeType attribute_type, Scope<ISlateAttributeGetter>&& wrapper);

		void protected_invalidate_widget(SWidget& widget, ESlateAttributeType attribute_type, EInvalidateWidgetReason invalidation_reason) const;

		bool protected_is_bound(const SWidget& widget, ESlateAttributeType attribute_type) const;

		ISlateAttributeGetter* protected_find_getter(const SWidget& widget, ESlateAttributeType attribute_type) const;

		void protected_update_now(SWidget& widget, ESlateAttributeType attribute_type);
	};
}
