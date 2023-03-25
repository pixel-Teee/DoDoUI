#pragma once

#include "SlateCore/Widgets/InvalidateWidgetReason.h"

#include "SlateCore/Widgets/SWidget.h"//TPaddingSingleWidgetSlotMixin depends on it

#include "SlateCore/Types/SlateAttribute.h"//TPaddingSingleWidgetSlotMixin depends on it

#include "SlateCore/Layout/Margin.h"//TPaddingSingleWidgetSlotMixin depends on it

//#include "SlateCore/Layout/Children.h"//TPaddingSingleWidgetSlotMixin depends on it

#include "SlateCore/Types/SlateEnums.h"//TAlignmentWidgetSlotMixin depends on it

#include "SlateCore/Layout/WidgetSlotWithAttributeSupport.h"//TBasicLayoutWidgetSlot depends on it

#include "SlateCore/Widgets/DeclarativeSyntaxSupport.h"//slot marco use this header

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
			friend class TAlignmentWidgetSlotMixin;

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
		//HAlign will be deprecated soon, use set vertical alignment or construct a new slot with FSlotArguments
		MixedIntoType& HAlign(EHorizontalAlignment in_halignment)
		{
			m_halignment = in_halignment;
			return *(static_cast<MixedIntoType*>(this));
		}

		//VAlign will be deprecated soon, use set vertical alignment or construct a new slot with FSlotArguments
		MixedIntoType& VAlign(EVerticalAlignment in_valignment)
		{
			m_valignment = in_valignment;
			return *(static_cast<MixedIntoType*>(this));
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

	/* mixin to add the alignment functionality to a base slot that is also a single children*/
	template<typename MixedIntoType>
	class TAlignmentSingleWidgetSlotMixin
	{
	public:
		template<typename WidgetType, typename V = typename std::enable_if<std::is_base_of<SWidget, WidgetType>::value>::type>
		TAlignmentSingleWidgetSlotMixin(WidgetType& in_parent)
			: m_halignment(HAlign_Fill)
			, m_valignment(VAlign_Fill)
		{}

		template<typename WidgetType, typename V = typename std::enable_if<std::is_base_of<SWidget, WidgetType>::value>::type>
		TAlignmentSingleWidgetSlotMixin(WidgetType& in_parent, const EHorizontalAlignment in_halign, const EVerticalAlignment in_valign)
			: m_halignment(HAlign_Fill)
			, m_valignment(VAlign_Fill)
		{}
	public:
		struct FSlotArgumentsMixin
		{
		public:
			friend TAlignmentSingleWidgetSlotMixin;

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
		//HAlign will be deprecated soon, use set vertical alignment or construct a new slot with FSlotArguments
		MixedIntoType& HAlign(EHorizontalAlignment in_halignment)
		{
			m_halignment = in_halignment;
			return *(static_cast<MixedIntoType*>(this));
		}

		//VAlign will be deprecated soon, use set vertical alignment or construct a new slot with FSlotArguments
		MixedIntoType& VAlign(EVerticalAlignment in_valignment)
		{
			m_valignment = in_valignment;
			return *(static_cast<MixedIntoType*>(this));
		}

	public:
		void set_horizontal_alignment(EHorizontalAlignment alignment)
		{
			if (m_halignment != alignment)
			{
				m_halignment = alignment;
				//call widget's Invalidate function
				static_cast<MixedIntoType*>(this)->invalidate(EInvalidateWidgetReason::Layout);
			}
		}

		EHorizontalAlignment get_horizontal_alignment() const
		{
			return m_halignment;
		}

		void set_vertical_alignment(EVerticalAlignment alignment)
		{
			if (m_valignment != alignment)
			{
				m_valignment = alignment;
				static_cast<MixedIntoType*>(this)->invalidate(EInvalidateWidgetReason::Layout);
			}
		}

		EVerticalAlignment get_vertical_alignment() const
		{
			return m_valignment;
		}
	public:
		EHorizontalAlignment m_halignment;

		EVerticalAlignment m_valignment;
	};

	/*mixin to add the padding functionality to a base slot*/
	template<typename MixedIntoType>
	class TPaddingWidgetSlotMixin
	{
	public:
		TPaddingWidgetSlotMixin()
			: m_slot_padding_attribute(*static_cast<MixedIntoType*>(this))
		{}
		TPaddingWidgetSlotMixin(const FMargin& margin)
			: m_slot_padding_attribute(*static_cast<MixedIntoType*>(this), margin)
		{}

	public:
		struct FSlotArgumentsMixin
		{
		private:
			friend TPaddingWidgetSlotMixin;
			TAttribute<FMargin> _padding;

		public:
			typename MixedIntoType::FSlotArguments& Padding(TAttribute<FMargin> in_padding)
			{
				_padding = std::move(in_padding);
				return static_cast<typename MixedIntoType::FSlotArguments&>(*this);
			}

			typename MixedIntoType::FSlotArguments& Padding(float uniform)
			{
				_padding = FMargin(uniform);
				return static_cast<typename MixedIntoType::FSlotArguments&>(*this);
			}

			typename MixedIntoType::FSlotArguments& Padding(float horizontal, float vertical)
			{
				_padding = FMargin(horizontal, vertical);
				return static_cast<typename MixedIntoType::FSlotArguments&>(*this);
			}

			typename MixedIntoType::FSlotArguments& Padding(float left, float top, float right, float bottom)
			{
				_padding = FMargin(left, top, right, bottom);
				return static_cast<typename MixedIntoType::FSlotArguments&>(*this);
			}
		};

		FMargin get_padding() const
		{
			return m_slot_padding_attribute.Get();
		}

	protected:
		void Construct_Mixin(const FChildren& slot_owner, FSlotArgumentsMixin&& in_args)
		{
			if(in_args._padding.Is_Set())
			{
				m_slot_padding_attribute.Assign(*static_cast<MixedIntoType*>(this), std::move(in_args._padding));//steal
			}
		}

		//todo:implement register attributes mixin

		//todo:implement set padding function
	private:
		using SlotPaddingCompareType = TSlateAttributeComparePredicate<>;
		//note:this is difference from TPaddingSingleWidgetSlotMixin, this is ContainedAttribute
		using SlotPaddingType = SlateAttributePrivate::TSlateContainedAttribute<FMargin, SlateAttributePrivate::FSlateAttributeNoInvalidationReason, SlotPaddingCompareType>;
		SlotPaddingType m_slot_padding_attribute;
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
			: m_slot_padding_attribute(in_parent, margin)
		{
			
		}

		MixedIntoType& Padding(TAttribute<FMargin> in_padding)
		{
			set_padding(std::move(in_padding));
			return *(static_cast<MixedIntoType*>(this));
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

	/*a templated basic slot that can be used by layout*/
	template<typename SlotType>
	class TBasicLayoutWidgetSlot : public TWidgetSlotWithAttributeSupport<SlotType>//suport container information query
		, public TPaddingWidgetSlotMixin<SlotType>
		, public TAlignmentWidgetSlotMixin<SlotType>
	{
	public:
		TBasicLayoutWidgetSlot()
			: TWidgetSlotWithAttributeSupport<SlotType>()
			, TPaddingWidgetSlotMixin<SlotType>()
			, TAlignmentWidgetSlotMixin<SlotType>()
		{}

		TBasicLayoutWidgetSlot(FChildren& in_owner)
			: TWidgetSlotWithAttributeSupport<SlotType>(in_owner)
			, TPaddingWidgetSlotMixin<SlotType>()
			, TAlignmentWidgetSlotMixin<SlotType>()
		{}

		TBasicLayoutWidgetSlot(const EHorizontalAlignment& in_halign, const EVerticalAlignment& in_valign)
			: TWidgetSlotWithAttributeSupport<SlotType>()
			, TPaddingWidgetSlotMixin<SlotType>()
			, TAlignmentWidgetSlotMixin<SlotType>(in_halign, in_valign)
		{}

		TBasicLayoutWidgetSlot(FChildren& in_owner, const EHorizontalAlignment& in_halign, const EVerticalAlignment& in_valign)
			: TWidgetSlotWithAttributeSupport<SlotType>(in_owner)
			, TPaddingWidgetSlotMixin<SlotType>()
			, TAlignmentWidgetSlotMixin<SlotType>(in_halign, in_valign)
		{}

	public:
		//inherited from three struct to generate a slot arguments struct
		SLATE_SLOT_BEGIN_ARGS_TwoMixins(TBasicLayoutWidgetSlot, TSlotBase<SlotType>, TPaddingWidgetSlotMixin<SlotType>, TAlignmentWidgetSlotMixin<SlotType>)
		SLATE_SLOT_END_ARGS()

		void Construct(const FChildren& slot_owner, FSlotArguments&& in_args)
		{
			TWidgetSlotWithAttributeSupport<SlotType>::Construct(slot_owner, std::move(in_args));
			TPaddingWidgetSlotMixin<SlotType>::Construct_Mixin(slot_owner, std::move(in_args));
			TAlignmentWidgetSlotMixin<SlotType>::Construct_Mixin(slot_owner, std::move(in_args));
		}

		static void register_attributes(FSlateWidgetSlotAttributeInitializer& attribute_initializer)
		{
			//TWidgetSlotWithAttributeSupport<SlotType>::
			//todo:implement register
			TPaddingSingleWidgetSlotMixin<SlotType>::Register_Attributes_Mixin(attribute_initializer);
		}
	};

	/*the basic slot that can be used by layout*/
	class FBasicLayoutWidgetSlot : public TBasicLayoutWidgetSlot<FBasicLayoutWidgetSlot>
	{
	public:
		using TBasicLayoutWidgetSlot<FBasicLayoutWidgetSlot>::TBasicLayoutWidgetSlot;
	};
}
