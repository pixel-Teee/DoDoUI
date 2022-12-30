#pragma once

#include "SlateCore/Widgets/InvalidateWidgetReason.h"

#include "SlateCore/Widgets/SWidget.h"//TPaddingSingleWidgetSlotMixin depends on it

#include "SlateCore/Types/SlateAttribute.h"//TPaddingSingleWidgetSlotMixin depends on it

#include "SlateCore/Layout/Margin.h"//TPaddingSingleWidgetSlotMixin depends on it

#include "SlateCore/Layout/Children.h"//TPaddingSingleWidgetSlotMixin depends on it

#include "SlateCore/Types/SlateEnums.h"//TAlignmentWidgetSlotMixin depends on it

namespace DoDo
{
	/* mixin to add the alignment functionality to a base slot */
	template<typename MixedIntoType>
	class TAlignmentWidgetSlotMixin
	{
	public:
		TAlignmentWidgetSlotMixin()
			: m_halignment(HAlign_Fill)
			, m_valignment(VAlign_Fill)
		{
			
		}

		TAlignmentWidgetSlotMixin(const EHorizontalAlignment In_HAlign, const EVerticalAlignment In_VAlign)
			: m_halignment(HAlign_Fill)
			, m_valignment(VAlign_Fill)
		{
			
		}
	public:
		struct FSlotArgumentsMixin
		{
		public:
			friend TAlignmentWidgetSlotMixin;

		public:
			typename MixedIntoType::FSlotArguments& HAlign(EHorizontalAlignment in_halignment)
			{
				m_halignment = in_halignment;
				return static_cast<typename MixedIntoType::FSlotArguments&>(*this);
			}

			typename MixedIntoType::FSlotArguments& VAlign(EVerticalAlignment in_valignment)
			{
				m_valignment = in_valignment;
				return static_cast<typename MixedIntoType::FSlotArguments&>(*this);
			}

		private:
			std::optional<EHorizontalAlignment> m_halignment;
			std::optional<EVerticalAlignment> m_valignment;
		};

	protected:
		void Construct_Mixin(const FChildren& slot_owner, FSlotArgumentsMixin&& in_args)
		{
			m_halignment = in_args.m_halignment.value_or(m_halignment);
			m_valignment = in_args.m_valignment.value_or(m_valignment);
		}

	public:
		void set_horizontal_alignment(EHorizontalAlignment alignment)
		{
			if(m_halignment != alignment)
			{
				m_halignment = alignment;
				//call widget's Invalidate function
				static_cast<MixedIntoType*>(this)->Invalidate(EInvalidateWidgetReason::Layout);
			}
		}

		EHorizontalAlignment get_horizontal_alignment() const
		{
			return m_halignment;
		}

		void set_vertical_alignment(EVerticalAlignment alignment)
		{
			if(m_valignment != alignment)
			{
				m_valignment = alignment;
				static_cast<MixedIntoType*>(this)->Invalidate(EInvalidateWidgetReason::Layout);
			}
		}

		EVerticalAlignment get_vertical_alignment() const
		{
			return m_valignment;
		}
	public:
		/*horizontal positioning of child within the allocated slot*/
		EHorizontalAlignment m_halignment;

		/*vertical positioning of child within the allocated slot*/
		EVerticalAlignment m_valignment;
	};

	/* mixin to add the padding functionality to a base slot that is also a single children */
	template<typename MixedIntoType, EInvalidateWidgetReason InPaddingInvalidationReason = EInvalidateWidgetReason::Layout>
	class TPaddingSingleWidgetSlotMixin
	{
	public:
		template<typename WidgetType, typename V = typename std::enable_if<std::is_base_of<SWidget, WidgetType>::value>::type>
		TPaddingSingleWidgetSlotMixin(WidgetType& in_parent)
			: m_slot_padding_attribute(in_parent)
		{
			
		}

		template<typename WidgetType, typename V = typename std::enable_if<std::is_base_of<SWidget, WidgetType>::value>::type>
		TPaddingSingleWidgetSlotMixin(WidgetType& in_parent, const FMargin& margin)
			: m_slot_padding_attribute(in_parent)
		{
			
		}
	public:
		struct FSlotArgumentsMixin
		{
		private:
			friend TPaddingSingleWidgetSlotMixin;
			TAttribute<FMargin> m_padding;//FSlotArgumentsMixin stores the FMargin, will move to TPaddingSingleWidgetSlotMixin

		public:
			typename MixedIntoType::FSlotArguments& Padding(TAttribute<FMargin> In_Padding)
			{
				m_padding = std::move(In_Padding);
				return static_cast<typename MixedIntoType::FSlotArguments&>(*this);//return to most derived FSlotArguments
			}

			typename MixedIntoType::FSlotArguments& Padding(float uniform)
			{
				m_padding = FMargin(1.0f);
				return static_cast<typename MixedIntoType::FSlotArguments&>(*this);
			}

			typename MixedIntoType::FSlotArguments& Padding(float Horizontal, float Vertical)
			{
				m_padding = FMargin(Horizontal, Vertical);
				return static_cast<typename MixedIntoType::FSlotArguments&>(*this);
			}

			typename MixedIntoType::FSlotArguments& Padding(float left, float top, float right, float bottom)
			{
				m_padding = FMargin(left, top, right, bottom);
				return static_cast<typename MixedIntoType::FSlotArguments&>(*this);
			}
		};

		void set_padding(TAttribute<FMargin> in_padding)
		{
			SWidget* owner_widget = static_cast<MixedIntoType*>(this)->get_owner_widget();//FSlotBase's function get_owner_widget
			
			//todo:check owner_widget
			m_slot_padding_attribute.Assign(*owner_widget, std::move(in_padding));
		}

		FMargin get_padding() const
		{
			return m_slot_padding_attribute.Get();
		}
	protected:
		void Construct_Mixin(const FChildren& slot_owner, FSlotArgumentsMixin&& in_args)
		{
			if(in_args.m_padding.Is_Set())
			{
				//steal the argument's padding to slot padding attribute
				m_slot_padding_attribute.Assign(*static_cast<MixedIntoType*>(this), std::move(in_args.m_padding));
			}
		}

		//FSlateWidgetSlotAttributeInitializer is FSlateAttributeDescriptor::FContainerInitializer
		static void Register_Attributes_Mixin(FSlateWidgetSlotAttributeInitializer& attribute_initializer)
		{
			//todo:implement Add Contained Attribute function
		}
	public:
		using SlotPaddingInvalidationType = typename std::conditional<InPaddingInvalidationReason == EInvalidateWidgetReason::None, SlateAttributePrivate::FSlateAttributeNoInvalidationReason, TSlateAttributeInvalidationReason<InPaddingInvalidationReason>>::type;
		using SlotPaddingAttributeType = SlateAttributePrivate::TSlateMemberAttribute<FMargin, SlotPaddingInvalidationType, TSlateAttributeComparePredicate<>>;//just a TSlateAttribute
		//using SlotPaddingAttributeRefType;
		//todo:implement TSlateMemberAttributeRef<SlotPaddingAttributeType>

		//template<typename WidgetType, typename V = typename std::enable_if<std::is_base_of<SWidget, WidgetType>::value>::type>
		//todo:implement GetSlotPaddingAttribute function
	protected:
		SlotPaddingAttributeType m_slot_padding_attribute;
	};
}