#pragma once

#include "ISlateMetaData.h"
#include "SlateAttribute.h"
#include "SlateCore/Widgets/InvalidateWidgetReason.h"

namespace DoDo {
	class SWidget;

	class FSlateAttributeMetaData : public ISlateMetaData
	{
		friend SlateAttributePrivate::FSlateAttributeImpl;
		

	public:
		SLATE_METADATA_TYPE(FSlateAttributeMetaData, ISlateMetaData)

		FSlateAttributeMetaData() = default;

		FSlateAttributeMetaData(const FSlateAttributeMetaData&) = delete;

		FSlateAttributeMetaData& operator=(const FSlateAttributeMetaData&) = delete;

	public:

		//return ths instance associated to the swidget(if it exists)
		static FSlateAttributeMetaData* find_meta_data(const SWidget& Owning_Widget);

		enum class EInvalidationPermission : uint8_t
		{
			//invalidate the widget if it's needed and it's construction phase is completed
			Allow_Invalidation_If_Constructed,

			//invalidate the widget if it's needed
			Allow_Invalidation,

			//cache the invalidation, on any future update, if it's needed, invalidate the widget
			Delay_Invalidation,

			//never invalidate the widget
			Deny_Invalidation,

			//never invalidate the widget and clear any delayed invalidation
			Deny_Any_Clear_Delayed_Invalidation
		};

		/*
			update all the attributes

			param : invalidation style if we should invalidate the widget
		*/
		static void update_all_attributes(SWidget& Owning_Widget, EInvalidationPermission In_Validation_Style);

	private:
		struct FGetterItem
		{
			FGetterItem(const FGetterItem&) = delete;
			FGetterItem(FGetterItem&&) = default;
			FGetterItem& operator=(const FGetterItem&) = delete;
			FGetterItem(FSlateAttributeBase* In_Attribute, uint32_t in_sort_order, Scope<SlateAttributePrivate::ISlateAttributeGetter>&& In_Getter);

			FSlateAttributeBase* m_attribute;
			std::unique_ptr<SlateAttributePrivate::ISlateAttributeGetter> m_getter;
			uint32_t m_sort_order;
			int32_t attribute_container_offset;//to fit inside a 32b structure, save the offset of the attribute container

			bool operator<(const FGetterItem& Other) const
			{
				//if in a conatiner, the smaller container wins first, then the property sort order

				return m_sort_order < Other.m_sort_order;
			}
		};

		void update_attributes_impl(SWidget& Owning_Widget, EInvalidationPermission In_Validation_Style, int32_t start_index, int32_t index_num);
	};
}