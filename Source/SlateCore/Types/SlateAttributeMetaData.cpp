#include <PreCompileHeader.h>

#include "SlateAttributeMetaData.h"

#include "SlateCore/Widgets/SWidget.h"

namespace DoDo {
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

	FDelegateHandle FSlateAttributeMetaData::Get_Attribute_Getter_Handle(const SWidget& Owning_Widget,
		const FSlateAttributeBase& attribute)
	{
		if(FSlateAttributeMetaData* attribute_meta_data = FSlateAttributeMetaData::find_meta_data(Owning_Widget))
		{
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
}