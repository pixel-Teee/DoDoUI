#pragma once

#include "ChildrenBase.h"//FChildren depends on it

#include "SlateCore/SlotBase.h"//TSlotBase depends on it

#include "SlateCore/Widgets/InvalidateWidgetReason.h"

#include "BasicLayoutWidgetSlot.h"//TSingleWidgetChildrenWithBasicLayoutSlot depends on it

#include "SlateCore/Widgets/DeclarativeSyntaxSupport.h"//TSingleWidgetChildrenWithBasicLayoutSlot depends on it

namespace DoDo
{
	/* a FChildren that has only one child and can take a templated slot */
	template<typename SlotType>
	class TSingleWidgetChildrenWithSlot : public FChildren, TSlotBase<SlotType>
	{
	public:
		TSingleWidgetChildrenWithSlot(SWidget* in_owner)
			: FChildren(in_owner)
			, TSlotBase<SlotType>(static_cast<const FChildren&>(*this))//let slot to access FChildren
		{
			
		}

		TSingleWidgetChildrenWithSlot(SWidget* in_owner, DoDoUtf8String in_name)
			: FChildren(in_owner, in_name)
			, TSlotBase<SlotType>(static_cast<const FChildren&>(*this))
		{
			
		}

		TSingleWidgetChildrenWithSlot(std::nullptr_t) = delete;

	public:
		//return the number of FChildren, just one
		virtual int32_t num() const override { return 1;  }

		virtual std::shared_ptr<SWidget> get_child_at(int32_t index) override
		{
			assert(index == 0);
			return this->get_widget();//FSlotBase's function
		}

		virtual std::shared_ptr<const SWidget> get_child_at(int32_t index) const override
		{
			assert(index == 0);
			return this->get_widget();
		}

	public:
		//inherited from a TSlotBase<SlotType>::FSlotArguments
		struct FSlotArguments : protected TSlotBase<SlotType>::FSlotArguments
		{
			FSlotArguments()
				: TSlotBase<SlotType>::FSlotArguments(TSlotBase<SlotType>::ConstructSlotIsFChildren)
			{
				
			}

			friend TSingleWidgetChildrenWithSlot;

			//note:return this type, have real type
			typename SlotType::FSlotArguments& operator[](const std::shared_ptr<SWidget>& in_child_widget)
			{
				TSlotBase<SlotType>::FSlotArguments::attach_widget(in_child_widget);
				return static_cast<typename SlotType::FSlotArguments&>(*this);
			}
		};

		void Construct(FSlotArguments&& in_args)
		{
			//steal FSlotArguments's argument to TSlotBase
			TSlotBase<SlotType>::Construct(*this, std::move(in_args));
		}
	public:
		TSlotBase<SlotType>&& As_Slot() { return *this; }

		const TSlotBase<SlotType>& As_Slot() const { return *this; }

		using TSlotBase<SlotType>::get_owner_widget;
		using TSlotBase<SlotType>::attach_widget;
		using TSlotBase<SlotType>::detach_widget;
		using TSlotBase<SlotType>::get_widget;
		using TSlotBase<SlotType>::invalidate;

		SlotType& operator[](const std::shared_ptr<SWidget>& in_child_widget)
		{
			this->attach_widget(in_child_widget);
			return static_cast<SlotType&>(*this);
		}

		SlotType& Expose(SlotType*& out_var_to_init)
		{
			out_var_to_init = static_cast<SlotType*>(*this);
			return static_cast<SlotType&>(*this);
		}

	private:
		virtual const FSlotBase& get_slot_at(int32_t child_index) const override
		{
			assert(child_index == 0);
			return *this;
		}

		//todo:implement return FWidgetRef
	};

	/* a FChildren that has only one child and support alignment and padding */
	template<EInvalidateWidgetReason In_Padding_Invalidation_Reason = EInvalidateWidgetReason::Layout>
	class TSingleWidgetChildrenWithBasicLayoutSlot : public TSingleWidgetChildrenWithSlot<TSingleWidgetChildrenWithBasicLayoutSlot<In_Padding_Invalidation_Reason>>
		, public TPaddingSingleWidgetSlotMixin<TSingleWidgetChildrenWithBasicLayoutSlot<In_Padding_Invalidation_Reason>, In_Padding_Invalidation_Reason>
		, public TAlignmentWidgetSlotMixin<TSingleWidgetChildrenWithBasicLayoutSlot<In_Padding_Invalidation_Reason>>
	{
	public:
		using ParentType = TSingleWidgetChildrenWithSlot<TSingleWidgetChildrenWithBasicLayoutSlot<In_Padding_Invalidation_Reason>>;//parent type
		using PaddingMixinType = TPaddingSingleWidgetSlotMixin<TSingleWidgetChildrenWithBasicLayoutSlot<In_Padding_Invalidation_Reason>, In_Padding_Invalidation_Reason>;
		using AlignmentMixinType = TAlignmentWidgetSlotMixin<TSingleWidgetChildrenWithBasicLayoutSlot<In_Padding_Invalidation_Reason>>;

	public:
		template<typename WidgetType, typename V = typename std::enable_if<std::is_base_of<SWidget, WidgetType>::value>::type>
		TSingleWidgetChildrenWithBasicLayoutSlot(WidgetType* in_owner)
			: ParentType(in_owner)
			, PaddingMixinType(in_owner)
			, AlignmentMixinType(*in_owner, HAlign_Fill, VAlign_Fill)
		{
			
		}

		template<typename WidgetType, typename V = typename std::enable_if<std::is_base_of<SWidget, WidgetType>::value>::type>
		TSingleWidgetChildrenWithBasicLayoutSlot(WidgetType* in_owner, const EHorizontalAlignment in_halign, const EVerticalAlignment in_valign)
			: ParentType(in_owner)
			, PaddingMixinType(*in_owner)
			, AlignmentMixinType(*in_owner, in_halign, in_valign)
		{
			
		}

		TSingleWidgetChildrenWithBasicLayoutSlot(std::nullptr_t) = delete;
		TSingleWidgetChildrenWithBasicLayoutSlot(std::nullptr_t, const EHorizontalAlignment in_halign, const EVerticalAlignment in_valign) = delete;

		//todo:implement SLATE_SLOT_BEGIN_ARGS_TwonMixins macro
		//this macro lets TSingleWidgetChildrenWithBasicLayoutSlot inherited from ParentType
		SLATE_SLOT_BEGIN_ARGS_TwoMixins(TSingleWidgetChildrenWithBasicLayoutSlot, ParentType, PaddingMixinType, AlignmentMixinType)
		SLATE_SLOT_END_ARGS()

		void Construct(FSlotArguments&& in_args)
		{
			ParentType::Construct(std::move(in_args));
			PaddingMixinType::Construct_Mixin(std::move(in_args));
			AlignmentMixinType::Construct_Mixin(std::move(in_args));
		}
	};
}
