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
		const FSlateAttributeDescriptor::FAttribute* found_member_attribute;
	}
}