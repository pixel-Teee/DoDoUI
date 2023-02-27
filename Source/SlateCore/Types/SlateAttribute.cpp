#include <PreCompileHeader.h>

#include "SlateAttribute.h"

#include "SlateAttributeMetaData.h"//note:include slate attribute meta data

namespace DoDo {
	namespace SlateAttributePrivate {
		void ISlateAttributeContainer::Remove_Container_Widget(SWidget& widget)
		{
			//todo:dispatch this function
		}

		void FSlateAttributeImpl::protected_unregister_attribute(SWidget& widget, ESlateAttributeType attribute_type) const
		{
			FSlateAttributeMetaData::UnRegister_Attribute(widget, *this);
		}
		void FSlateAttributeImpl::protected_register_attribute(SWidget& widget, ESlateAttributeType attribute_type, Scope<ISlateAttributeGetter>&& wrapper)
		{
			FSlateAttributeMetaData::Register_Attribute(widget, *this, attribute_type, std::move(wrapper));
		}
		void FSlateAttributeImpl::protected_invalidate_widget(SWidget& widget, ESlateAttributeType attribute_type, EInvalidateWidgetReason invalidation_reason) const
		{
			FSlateAttributeMetaData::In_Validate_Widget(widget, *this, attribute_type, invalidation_reason);
		}
		bool FSlateAttributeImpl::protected_is_bound(const SWidget& widget, ESlateAttributeType attribute_type) const
		{
			return false;
		}
		ISlateAttributeGetter* FSlateAttributeImpl::protected_find_getter(const SWidget& widget, ESlateAttributeType attribute_type) const
		{
			return nullptr;//todo:implement this function
		}

		FDelegateHandle FSlateAttributeImpl::protected_find_getter_handle(const SWidget& widget,
			ESlateAttributeType attribute_type) const
		{
			//static function
			//to find widget slate meta data is registered this slate attribute?
			return FSlateAttributeMetaData::Get_Attribute_Getter_Handle(widget, *this);
		}

		void FSlateAttributeImpl::protected_register_attribute(ISlateAttributeContainer& container,
			ESlateAttributeType attribute_type, Scope<ISlateAttributeGetter>&& wrapper)
		{
			FSlateAttributeMetaData::Register_Attribute(container, *this, attribute_type, std::move(wrapper));
		}

		void FSlateAttributeImpl::protected_unregister_attribute(ISlateAttributeContainer& container,
			ESlateAttributeType attribute_type) const
		{
			FSlateAttributeMetaData::UnRegister_Attribute(container, *this);
		}

		void FSlateAttributeImpl::protected_invalidate_widget(ISlateAttributeContainer& container,
			ESlateAttributeType attribute_type, EInvalidateWidgetReason invalidation_reason) const
		{
			FSlateAttributeMetaData::In_Validate_Widget(container, *this, attribute_type, invalidation_reason);
		}

		FDelegateHandle FSlateAttributeImpl::protected_find_getter_handle(const ISlateAttributeContainer& container,
		                                                                  ESlateAttributeType attribute_type) const
		{
			//note:get parent widget
			return protected_find_getter_handle(container.Get_Container_Widget(), attribute_type);
		}

		void FSlateAttributeImpl::protected_update_now(SWidget& widget, ESlateAttributeType attribute_type)
		{
			FSlateAttributeMetaData::update_attribute(widget, *this);
		}
	}
	
}


