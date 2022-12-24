#pragma once

#include "ISlateMetaData.h"
#include "SlateAttribute.h"
#include <SlateCore/Widgets/InvalidateWidgetReason.h>

#include "SlateAttributeDescriptor.h"//FGetterItem depends on it

namespace DoDo {
	class SWidget;
	//class DoDoUtf8String;
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
		using ESlateAttributeType = SlateAttributePrivate::ESlateAttributeType;
		using ISlateAttributeGetter = SlateAttributePrivate::ISlateAttributeGetter;

		//register a FSlateAttributeBase ISlateAttributeGetter to SWidget slate meta's array
		static void Register_Attribute(SWidget& owning_widget, FSlateAttributeBase& attribute, ESlateAttributeType attribute_type, std::unique_ptr<ISlateAttributeGetter>&& wrapper);
		static bool UnRegister_Attribute(SWidget& owning_widget, const FSlateAttributeBase& attribute);
		static void In_Validate_Widget(SWidget& owning_widget, const FSlateAttributeBase& attribute, ESlateAttributeType attribute_type, EInvalidateWidgetReason reason);
		static FDelegateHandle Get_Attribute_Getter_Handle(const SWidget& Owning_Widget, const FSlateAttributeBase& attribute);

	private:
		void update_attributes_impl(SWidget& Owning_Widget, EInvalidationPermission In_Validation_Style, int32_t start_index, int32_t index_num);

		void register_member_attribute_impl(SWidget& Owning_Widget, FSlateAttributeBase& In_Validation_Style, std::unique_ptr<ISlateAttributeGetter>&& getter);

		bool unregister_attribute_impl(const FSlateAttributeBase& attribute);

		void remove_meta_data_if_needed(SWidget& owning_widget, bool b_Removed) const;
	private:
		int32_t Index_Of_Attribute(const FSlateAttributeBase& attribute) const
		{
			const FSlateAttributeBase* attribute_ptr = &attribute;
			auto iter = std::find_if(m_attributes.begin(), m_attributes.end(), [attribute_ptr](const FGetterItem& item)
			{
				return item.m_attribute == attribute_ptr;
			});

			//-1 indicates none
			if (iter == m_attributes.end()) return -1;

			//todo:may have some bug
			return std::distance(m_attributes.begin(), iter);
		}

		struct FGetterItem
		{
			FGetterItem(const FGetterItem&) = delete;
			FGetterItem(FGetterItem&&) = default;
			//todo:need to fix this?, vector use = to copy
			FGetterItem& operator=(const FGetterItem&) = delete;
			//todo:implement this check
			FGetterItem& operator=(FGetterItem&& rhs)
			{
				//if(this != &rhs)
				//{
				//	
				//}
				m_attribute = rhs.m_attribute;
				m_getter = std::move(rhs.m_getter);
				m_cached_attribute_descriptor = rhs.m_cached_attribute_descriptor;
				m_sort_order = rhs.m_sort_order;
				attribute_container_offset = rhs.attribute_container_offset;

				return *this;
			}
			FGetterItem(FSlateAttributeBase* In_Attribute, uint32_t in_sort_order, Scope<SlateAttributePrivate::ISlateAttributeGetter>&& In_Getter)
				: m_attribute(In_Attribute)
				, m_getter(std::move(In_Getter))
				, m_cached_attribute_descriptor(nullptr)
				, m_sort_order(in_sort_order)
				, attribute_container_offset(0)
			{
				
			}

			FGetterItem(FSlateAttributeBase* In_Attribute, uint32_t in_sort_order, Scope<SlateAttributePrivate::ISlateAttributeGetter>&& In_Getter, const FSlateAttributeDescriptor::FAttribute* in_attribute_descriptor)
			: m_attribute(In_Attribute)
			, m_getter(std::move(In_Getter))
			, m_cached_attribute_descriptor(in_attribute_descriptor)
			, m_sort_order(in_sort_order)
			, attribute_container_offset(0)
			{
				
			}

			FSlateAttributeBase* m_attribute;
			std::unique_ptr<SlateAttributePrivate::ISlateAttributeGetter> m_getter;
			const FSlateAttributeDescriptor::FAttribute* m_cached_attribute_descriptor;//describes FSlateAttributeBase class static information
			uint32_t m_sort_order;
			int32_t attribute_container_offset;//to fit inside a 32b structure, save the offset of the attribute container

			bool operator<(const FGetterItem& Other) const
			{
				//if in a conatiner, the smaller container wins first, then the property sort order

				return m_sort_order < Other.m_sort_order;
			}
		};

		std::vector<FGetterItem> m_attributes;

		/*
		 * there is a possibility that the widget has a cached invalidation reason and a parent become collapsed
		 * the invalidation will probably never get executed but
		 * 1. the widget is collapsed indirectly, so we do not care if it's invalidated
		 * 2. the parent widget will clear this widget persistent state
		 */
		EInvalidateWidgetReason m_cached_invalidation_reason = EInvalidateWidgetReason::None;

		uint8_t m_affect_visibility_counter = 0;
	};

}
