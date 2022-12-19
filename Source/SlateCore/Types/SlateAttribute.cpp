#include <PreCompileHeader.h>

#include "SlateAttribute.h"

#include "SlateAttributeMetaData.h"

namespace DoDo {
	namespace SlateAttributePrivate {
		void FSlateAttributeImpl::protected_unregister_attribute(SWidget& widget, ESlateAttributeType attribute_type) const
		{
			//test attribute address
			//...


		}
		void FSlateAttributeImpl::protected_register_attribute(SWidget& widget, ESlateAttributeType attribute_type, Scope<ISlateAttributeGetter>&& wrapper)
		{
		}
		void FSlateAttributeImpl::protected_invalidate_widget(SWidget& widget, ESlateAttributeType attribute_type, EInvalidateWidgetReason invalidation_reason) const
		{
		}
		bool FSlateAttributeImpl::protected_is_bound(const SWidget& widget, ESlateAttributeType attribute_type) const
		{
			return false;
		}
		ISlateAttributeGetter* FSlateAttributeImpl::protected_find_getter(const SWidget& widget, ESlateAttributeType attribute_type) const
		{
			return nullptr;
		}

		FDelegateHandle FSlateAttributeImpl::protected_find_getter_handle(const SWidget& widget,
			ESlateAttributeType attribute_type) const
		{
			return FSlateAttributeMetaData::Get_Attribute_Getter_Handle(widget, *this);
		}

		void FSlateAttributeImpl::protected_update_now(SWidget& widget, ESlateAttributeType attribute_type)
		{
		}
	}
	
}


