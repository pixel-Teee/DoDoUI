#include <PreCompileHeader.h>

#include "SlateAttributeMetaData.h"

#include "SlateCore/Widgets/SWidget.h"

#include "SlateAttributeDescriptor.h"

namespace DoDo {
	namespace Private
	{
		FSlateAttributeDescriptor::OffsetType find_member_offset(const SWidget& owning_widget, const FSlateAttributeBase& attribute)
		{
			uint64_t offset = (uint64_t)(&attribute) - (uint64_t)(&owning_widget);

			//OffsetType is uint32_t
			return (FSlateAttributeDescriptor::OffsetType)(offset);
		}

		EInvalidateWidgetReason invalidate_for_member(SWidget& owning_widget, const FSlateAttributeBase& attribute, EInvalidateWidgetReason reason)
		{
			const FSlateAttributeDescriptor::OffsetType offset = find_member_offset(owning_widget, attribute);
			if(const FSlateAttributeDescriptor::FAttribute* found_attribute = owning_widget.Get_Widget_Class().Get_Attribute_Descriptor().find_member_attribute(offset))
			{
				//call FSlateAttributeBase corresponding FAttribute in FSlateAttributeDescriptor's function
				found_attribute->Execute_On_Value_Changed(owning_widget);
				return found_attribute->Get_Invalidation_Reason(owning_widget);
			}
			return reason;
		}
	}
	FSlateAttributeMetaData* FSlateAttributeMetaData::find_meta_data(const SWidget& Owning_Widget)
	{
		if (Owning_Widget.Has_Registered_Slate_Attribute())
		{
			assert(Owning_Widget.m_Meta_Data.size() > 0);

			const ISlateMetaData& slate_meta_data = *(Owning_Widget.m_Meta_Data[0]);

			//check slate meta data's type is FSlateAttributeMetaData
			assert(slate_meta_data.Is_Of_Type<FSlateAttributeMetaData>());

			return static_cast<FSlateAttributeMetaData*>(const_cast<ISlateMetaData*>(&slate_meta_data));
		}

		return nullptr;
	}
	void FSlateAttributeMetaData::update_all_attributes(SWidget& Owning_Widget, EInvalidationPermission In_Validation_Style)
	{
		if (FSlateAttributeMetaData* attribute_meta_data = FSlateAttributeMetaData::find_meta_data(Owning_Widget))
		{
			
		}
	}

	void FSlateAttributeMetaData::Register_Attribute(SWidget& owning_widget, FSlateAttributeBase& attribute,
		ESlateAttributeType attribute_type, std::unique_ptr<ISlateAttributeGetter>&& wrapper)
	{
		auto Execute_Register = [&](FSlateAttributeMetaData& attribute_meta_data)
		{
			if (attribute_type == ESlateAttributeType::Member)
			{
				attribute_meta_data.register_member_attribute_impl(owning_widget, attribute, std::move(wrapper));
			}
			//todo:solve other ESlateAttributeType::Manager
		};

		//if swidget already have this slate meta data, it will be execute another branch
		if(FSlateAttributeMetaData* attribute_meta_data = FSlateAttributeMetaData::find_meta_data(owning_widget))
		{
			if(int32_t found_index = attribute_meta_data->Index_Of_Attribute(attribute); found_index != -1)
			{
				attribute_meta_data->m_attributes[found_index].m_getter = std::move(wrapper);
			}
			else
			{
				//execute register
				Execute_Register(*attribute_meta_data);
			}
		}
		else
		{
			//construct new element to slate meta data array
			std::shared_ptr<FSlateAttributeMetaData> new_attribute_meta_data = std::make_shared<FSlateAttributeMetaData>();

			Execute_Register(*new_attribute_meta_data);

		}
	}

	void FSlateAttributeMetaData::Register_Attribute(ISlateAttributeContainer& owning_container,
		FSlateAttributeBase& attribute, ESlateAttributeType attribute_type,
		std::unique_ptr<ISlateAttributeGetter>&& wrapper)
	{
		//todo:implement this function
	}

	bool FSlateAttributeMetaData::UnRegister_Attribute(SWidget& owning_widget, const FSlateAttributeBase& attribute)
	{
		if(FSlateAttributeMetaData* attribute_meta_data = FSlateAttributeMetaData::find_meta_data(owning_widget))
		{
			const bool b_Result = attribute_meta_data->unregister_attribute_impl(attribute);

			attribute_meta_data->remove_meta_data_if_needed(owning_widget, b_Result);

			return b_Result;
		}

		return false;
	}

	bool FSlateAttributeMetaData::UnRegister_Attribute(ISlateAttributeContainer& owning_container,
		const FSlateAttributeBase& attribute)
	{
		SWidget& owning_widget = owning_container.Get_Container_Widget();
		return UnRegister_Attribute(owning_widget, attribute);
	}

	void FSlateAttributeMetaData::In_Validate_Widget(SWidget& owning_widget, const FSlateAttributeBase& attribute,
	                                                 ESlateAttributeType attribute_type, EInvalidateWidgetReason reason)
	{
		/*
		 * the widget is in the construction phase or is building in the widget list
		 * it's already invalidated... no need to keep invalidating it
		 * no needs to set the bUpdatedManually in this case because
		 * 1.they are in construction, so they will all be called anyway
		 * 2.they are in Widget List, so the SlateAttribute.Set will not be called
		 */
		if(!owning_widget.Is_Constructed())
		{
			return;
		}

		const FSlateAttributeDescriptor::FAttributeValueChangedDelegate* on_value_changed_call_back = nullptr;

		//from attribute descriptor to collocate invalidation reason
		if(FSlateAttributeMetaData* attribute_meta_data = FSlateAttributeMetaData::find_meta_data(owning_widget))
		{
			const int32_t found_index = attribute_meta_data->Index_Of_Attribute(attribute);

			if(found_index != -1)
			{
				const FGetterItem& getter_item = attribute_meta_data->m_attributes[found_index];
				if(getter_item.m_cached_attribute_descriptor)
				{
					getter_item.m_cached_attribute_descriptor->Execute_On_Value_Changed(owning_widget);
					reason = getter_item.m_cached_attribute_descriptor->Get_Invalidation_Reason(owning_widget);
				}
			}
			else if(attribute_type == ESlateAttributeType::Member)
			{
				//todo:implement invalidate for member
				reason = Private::invalidate_for_member(owning_widget, attribute, reason);
			}

			reason |= attribute_meta_data->m_cached_invalidation_reason;
			attribute_meta_data->m_cached_invalidation_reason = EInvalidateWidgetReason::None;
		}
		else if(attribute_type == ESlateAttributeType::Member)
		{
			//todo:implement invalidate for member
			reason = Private::invalidate_for_member(owning_widget, attribute, reason);
		}

		//pass reason to widget invalidate function, interms of reason to mark different flag
		owning_widget.Invalidate(reason);
	}

	void FSlateAttributeMetaData::In_Validate_Widget(ISlateAttributeContainer& owning_container,
		const FSlateAttributeBase& attribute, ESlateAttributeType attribute_type, EInvalidateWidgetReason reason)
	{
		//todo:implement this function
	}

	void FSlateAttributeMetaData::update_attribute(SWidget& owning_widget, FSlateAttributeBase& attribute)
	{
		if(FSlateAttributeMetaData* attribute_meta_data = FSlateAttributeMetaData::find_meta_data(owning_widget))
		{
			const int32_t found_index = attribute_meta_data->Index_Of_Attribute(attribute);
			if(found_index != -1)
			{
				FGetterItem& getter_item = attribute_meta_data->m_attributes[found_index];

				ISlateAttributeGetter::FUpdateAttributeResult result = getter_item.m_getter->Update_Attribute(owning_widget);

				if(result.m_b_Invalidation_Requested && owning_widget.Is_Constructed())
				{
					EInvalidateWidgetReason invalidation_reason = result.m_Invalidation_Reason;
					if(getter_item.m_cached_attribute_descriptor)
					{
						getter_item.m_cached_attribute_descriptor->Execute_On_Value_Changed(owning_widget);
						invalidation_reason = getter_item.m_cached_attribute_descriptor->Get_Invalidation_Reason(owning_widget);
					}
					//todo:add invalidation flags
				}
			}
		}
	}

	FDelegateHandle FSlateAttributeMetaData::Get_Attribute_Getter_Handle(const SWidget& Owning_Widget,
	                                                                     const FSlateAttributeBase& attribute)
	{
		//just to get the first meta data
		if(FSlateAttributeMetaData* attribute_meta_data = FSlateAttributeMetaData::find_meta_data(Owning_Widget))
		{
			//to find parameter attribute index
			const int32_t found_index = attribute_meta_data->Index_Of_Attribute(attribute);//found index
			if(found_index != -1)
			{
				return attribute_meta_data->m_attributes[found_index].m_getter->Get_Delegate_Handle();
			}
		}

		return FDelegateHandle();
	}

	void FSlateAttributeMetaData::update_attributes_impl(SWidget& Owning_Widget, EInvalidationPermission In_Validation_Style, int32_t start_index, int32_t index_num)
	{
		bool b_invalidate_if_needed = (In_Validation_Style == EInvalidationPermission::Allow_Invalidation) || (In_Validation_Style == EInvalidationPermission::Allow_Invalidation_If_Constructed);
		bool b_allow_invalidation = b_invalidate_if_needed || In_Validation_Style == EInvalidationPermission::Delay_Invalidation;

		EInvalidateWidgetReason InValidation_Reason = EInvalidateWidgetReason::None;

		for (int32_t index = start_index; index < index_num; ++index)
		{
			
		}
	}
	void FSlateAttributeMetaData::register_member_attribute_impl(SWidget& Owning_Widget, FSlateAttributeBase& attribute, std::unique_ptr<ISlateAttributeGetter>&& getter)
	{
		//todo:need to complete this function
		//member attribute are optional for now but will be needed in the future
		const FSlateAttributeDescriptor::OffsetType attribute_offset = Private::find_member_offset(Owning_Widget, attribute);

		//todo:implement FSlateWidgetClassData
		const FSlateAttributeDescriptor::FAttribute* found_member_attribute = Owning_Widget.Get_Widget_Class().Get_Attribute_Descriptor().find_member_attribute(attribute_offset);

		if(found_member_attribute)
		{
			//todo:need to fix this?
			//ISlateMetaData have FGetterItem array(m_attributes)

			//binary search
			uint32_t l = 0, r = m_attributes.size() - 1;
			while(l < r)
			{
				uint32_t mid = l + r >> 1;

				uint32_t sort_order = m_attributes[mid].m_sort_order;//sort order

				if (found_member_attribute->Get_Sort_Order() >= sort_order) r = mid;
				else l = mid + 1;
			}

			//construct a temp rvalue to array
			//this will add FAttribute to FGetterItem
			m_attributes.insert(m_attributes.begin() + l, { &attribute, found_member_attribute->Get_Sort_Order(), std::move(getter), found_member_attribute });

			//FGetterItem& getter_item = *iter;

			if(found_member_attribute->Does_Affect_Visibility())
			{
				++m_affect_visibility_counter;
			}
		}
		else
		{
			//attribute offset * 100
			const uint32_t calculated_sort_order = FSlateAttributeDescriptor::Default_Sort_Order(attribute_offset);

			int32_t l = 0, r = (int32_t)(m_attributes.size()) - 1;
			while(l < r)
			{
				int32_t mid = l + r >> 1;

				int32_t sort_order = m_attributes[mid].m_sort_order;//sort order

				if (calculated_sort_order >= sort_order) r = mid;
				else l = mid + 1;
			}

			m_attributes.insert(m_attributes.begin() + l, { &attribute, calculated_sort_order, std::move(getter) });
		}
	}

	bool FSlateAttributeMetaData::unregister_attribute_impl(const FSlateAttributeBase& attribute)
	{
		const int32_t found_index = Index_Of_Attribute(attribute);

		if(found_index != -1)
		{
			//what cached attribute descriptor it is?
			if(m_attributes[found_index].m_cached_attribute_descriptor && m_attributes[found_index].m_cached_attribute_descriptor->Does_Affect_Visibility())
			{
				if (m_affect_visibility_counter > 0)
					--m_affect_visibility_counter;
			}
			//keep the order valid
			//from FGetterItem to erase it
			m_attributes.erase(m_attributes.begin() + found_index);
			return true;
		}

		return false;
	}

	void FSlateAttributeMetaData::remove_meta_data_if_needed(SWidget& owning_widget, bool b_Removed) const
	{
		//when getter item is empty
		if(m_attributes.size() == 0)
		{
			owning_widget.m_b_has_registered_slate_attribute = false;
			owning_widget.m_Meta_Data.erase(owning_widget.m_Meta_Data.begin());//erase first element
			if(owning_widget.Is_Constructed() && owning_widget.Is_Attributes_Updates_Enabled())
			{
				//call invalidate to mark different flag
				owning_widget.Invalidate(EInvalidateWidgetReason::Attribute_Registration);
			}
		}
	}
}
