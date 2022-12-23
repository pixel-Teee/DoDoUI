#pragma once

#include "Core/Delegates/Delegates.h"//FAttributeValueChangedDelegate depends on it
#include "SlateCore/Widgets/SWidget.h"

namespace DoDo
{
#define STRUCT_OFFSET(s, m) ((int32_t)(&((s*)0)->m))
	/*
	 * describes the static information about a widget's type slate attributes
	 */
	class FSlateAttributeDescriptor
	{
	public:
		/*
		 * a EInvalidationWidgetReason attribute
		 * it can be explicitly initialize or can be callback static function or lambda that returns the EInvalidationReason
		 * the signature of the function takes a const SWidget& as argument
		 */
		struct FInvalidateWidgetReasonAttribute
		{
		public:
			/*
			 * a EInvalidationWidgetReason Attribute
			 * it can be explicitly initialize or can be a callback static function or lambda that returns the EInvalidationReason
			 * the signature of the function takes a const SWidget& as argument
			 */
			friend FSlateAttributeDescriptor;

			using Arg1Type = const class SWidget&;

			//delegate, return EInvalidateWidgetReason, parameter is Arg1Type
			using FGetter = Delegate<EInvalidateWidgetReason(Arg1Type)>;

			FInvalidateWidgetReasonAttribute(const FInvalidateWidgetReasonAttribute&) = default;
			FInvalidateWidgetReasonAttribute(FInvalidateWidgetReasonAttribute&&) = default;
			FInvalidateWidgetReasonAttribute& operator=(const FInvalidateWidgetReasonAttribute&) = default;
			FInvalidateWidgetReasonAttribute& operator=(FInvalidateWidgetReasonAttribute&&) = default;

			//todo:in the future, add pay load to delegate
			explicit FInvalidateWidgetReasonAttribute(FGetter::Static_Function_Type in_func_ptr)
				: m_reason(EInvalidateWidgetReason::None)
				, m_getter(FGetter::Create(nullptr, in_func_ptr))
			{
				
			}

			bool Is_Bound() const
			{
				return m_getter.Is_Bound();
			}

			EInvalidateWidgetReason Get(const SWidget& widget) const
			{
				return Is_Bound() ? m_getter.Execute(widget) : m_reason;
			}

		private:
			EInvalidateWidgetReason m_reason;
			FGetter m_getter;
		};

		using FAttributeValueChangedDelegate = Delegate<void(SWidget&)>;

		enum class ECallbackOverrideType
		{
			//replace the callback that the base class defined
			Replace_Previous,
			//execute the callback that the base class defined, then execute the new callback
			Execute_After_Previous,
			//execute the new call back, then execute the callback that the base class defined
			Execute_Before_Previous
		};

		using OffsetType = uint32_t;

		struct FContainer
		{
		public:
			FContainer() = default;

			FContainer(DoDoUtf8String in_name, OffsetType in_offset)
				: m_name(in_name), m_offset(in_offset)
			{
				
			}

			bool Is_Valid() const
			{
				return !m_name.get_count();
			}

			DoDoUtf8String Get_Name() const
			{
				return m_name;
			}

			uint32_t Get_Sort_Order()
			{
				return m_sort_order;
			}
		private:
			DoDoUtf8String m_name;
			OffsetType m_offset = 0;
			uint32_t m_sort_order = 0;
		};

		struct FAttribute
		{
		public:
			//OffsetType = uint32_t
			FAttribute(DoDoUtf8String name, OffsetType offset, FInvalidateWidgetReasonAttribute reason);

			FAttribute(DoDoUtf8String container_name, DoDoUtf8String name, OffsetType offset, FInvalidateWidgetReasonAttribute reason);

			DoDoUtf8String Get_Name() const
			{
				return m_name;
			}

			uint32_t Get_Sort_Order()
			{
				return m_Sort_Order;
			}

			EInvalidateWidgetReason Get_Invalidation_Reason(const SWidget& widget) const
			{
				//return m_invalidation_reason.
				return  EInvalidateWidgetReason();//todo:need to complete this function
			}

			SlateAttributePrivate::ESlateAttributeType Get_Attribute_Type() const
			{
				return m_attribute_type;
			}

			bool Does_Affect_Visibility() const
			{
				return m_b_affect_visibility;
			}

			void Execute_On_Value_Changed(SWidget& widget) const
			{
				m_on_value_changed.Execute(widget);
			}

		private:
			DoDoUtf8String m_name;

			OffsetType m_offset;

			DoDoUtf8String m_perquisite;

			uint32_t m_Sort_Order;

			uint8_t m_Container_Index;

			FInvalidateWidgetReasonAttribute m_invalidation_reason;

			FAttributeValueChangedDelegate m_on_value_changed;

			SlateAttributePrivate::ESlateAttributeType m_attribute_type;

			bool m_b_affect_visibility;
		};

		/*internal class to initialize the SlateAttributeDescriptor::FContainer attributes(Add attributes or modify existing attributes)*/
		struct FContainerInitializer
		{
		private:
			friend FSlateAttributeDescriptor;

			FContainerInitializer(FSlateAttributeDescriptor& in_descriptor, DoDoUtf8String container_name);

			//parent descriptor?
			FContainerInitializer(FSlateAttributeDescriptor& in_descriptor, const FSlateAttributeDescriptor& parent_descriptor, DoDoUtf8String container_name);

		public:
			FContainerInitializer() = delete;

			FContainerInitializer(const FContainerInitializer&) = delete;

			FContainerInitializer& operator=(const FContainerInitializer&) = delete;

			struct FAttributeEntry
			{
				FAttributeEntry(FSlateAttributeDescriptor& descriptor, DoDoUtf8String container_name, int32_t atttribute_index);

				/*
				 * update the attribute after the prerequisite
				 * the order is guaranteed but other attributes may be updated in between
				 * no order is guaranteed if the prerequisite or this property is updated manually
				 */
				FAttributeEntry& update_prerequisite(DoDoUtf8String prerequisite);

				/*
				 * notified when the attribute value changed
				 * it's preferable that you delay any action to the Tick or Paint function
				 * you are not allowed to make changes that would affect the swidget child order or it's visibility
				 * it will not be called when the swidget is in his construction phase
				 * @see SWidget::IsConstructed
				 */
				FAttributeEntry& on_value_changed(FAttributeValueChangedDelegate callback);

			private:
				FSlateAttributeDescriptor& m_descriptor;
				DoDoUtf8String m_container_name;
				int32_t m_attribute_index;
			};

			FAttributeEntry add_contained_attribute(DoDoUtf8String attribute_name, OffsetType offset, const FInvalidateWidgetReasonAttribute& reason_getter);

			FAttributeEntry add_contained_attribute(DoDoUtf8String attribute_name, OffsetType offset, FInvalidateWidgetReasonAttribute&& reason_getter);

		public:
			//change the invalidation reason of an attribute defined in a base class
			void Override_Invalidation_Reason(DoDoUtf8String attribute_name, const FInvalidateWidgetReasonAttribute& reason);

			void Override_Invalidation_Reason(DoDoUtf8String attribute_name, FInvalidateWidgetReasonAttribute&& reason);

			//change the FAttributeValueChangedDelegate of an attribute defined in a base class
			void Override_On_Value_Changed(DoDoUtf8String attribute_name, ECallbackOverrideType override_type, FAttributeValueChangedDelegate call_back);

		private:
			FSlateAttributeDescriptor& m_descriptor;

			DoDoUtf8String m_container_name;
		};
		/*internal class to initialize the slate attribute descriptor (add attributes or modify existing attributes)*/
		struct FInitializer
		{
		private:
			friend class FSlateWidgetClassData;

			FInitializer(FSlateAttributeDescriptor& in_descriptor);

			FInitializer(FSlateAttributeDescriptor& in_descriptor, const FSlateAttributeDescriptor& parent_descriptor);

			FInitializer(const FInitializer&) = delete;

			FInitializer& operator=(const FInitializer&) = delete;

		public:
			~FInitializer();

			struct FAttributeEntry
			{
				FAttributeEntry(FSlateAttributeDescriptor& descriptor, int32_t in_attribute_index);

				/*
				 * update the attribute after the prerequisite
				 * the order is guaranteed but other attributes may be updated in between
				 * no order is guaranteed if the prerequisite or this property is updated manually
				 */
				FAttributeEntry& update_prerequisite(DoDoUtf8String pre_requisite);

				/*
				 * the attribute affect the visibility of the widget
				 * we only update the attributes that can change the visibility of the widget when the widget is collapsed
				 * attributes that affect visibility must have the visibility attribute as a prerequisite or the visibility attribute must have it as a prerequisite
				 */
				FAttributeEntry& affect_visibility();

				/*
				 * notified when the attribute value changed
				 * it's preferable that you delay any action to the tick or paint function
				 * you are not allowed to make changes that would affect the swidget child order or it's visibility
				 * it will not be called when the swidget is in his construction phase
				 * @see SWidget::IsConstructed
				 */
				FAttributeEntry& on_value_changed(FAttributeValueChangedDelegate call_back);

			private:
				FSlateAttributeDescriptor& m_descriptor;
				int32_t m_attribute_index;
			};

			FAttributeEntry add_member_attribute(DoDoUtf8String attribute_name, OffsetType offset, const FInvalidateWidgetReasonAttribute& reason_getter);

			FAttributeEntry add_member_attribute(DoDoUtf8String attribute_name, OffsetType offset, FInvalidateWidgetReasonAttribute&& reason_getter);

			FContainerInitializer add_container(DoDoUtf8String container_name, OffsetType offset);

		public:

		private:
			FSlateAttributeDescriptor& m_descriptor;
		};
		//todo:implement function
	private:

		std::vector<FAttribute> m_attributes;

		std::vector<FContainer> m_containers;
	};

	using FSlateAttributeInitializer = FSlateAttributeDescriptor::FInitializer;
	//slot attribute initializer ? 
	using FSlateWidgetSlotAttributeInitializer = FSlateAttributeDescriptor::FContainerInitializer;

	/*
	 * add a TSlateAttribute to the descriptor
	 * @param _Initializer the FSlateAttributeInitializer from the PrivateRegisterAttributes function
	 * @param _Property the TSlateAttribute property
	 * @param _Reason the EInvalidationWidgetReason or a static function/lambda that takes a const SWidget& and that returns the invalidation reason
	 */
#define SLATE_ADD_MEMBER_ATTRIBUTE_DEFINITION_WITH_NAME(_Initializer, _Name, _Property, _Reason) \
	_Initializer.AddMemberAttribute(_Name, STRUCT_OFFSET(PrivateThisType, _Property), FSlateAttributeDescriptor::FInvalidateWidgetReasonAttribute{_Reason})
}
