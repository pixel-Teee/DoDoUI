#pragma once

#include "ChildrenBase.h"

#include "SlateCore/SlotBase.h"

#include "SlateCore/Types/SlateAttribute.h"

#include "SlateCore/Widgets/SWidget.h"

/*
 * a base slot that supports TSlateContainedAttribute
 * the FChildren that own the FSlot also needs to support SlateContainedAttribute
 * @see FChildren::SupportSlotWithSlateAttribute
 */

namespace DoDo
{
	//note:a slot base inherited from ISlateAttributeContainer, have some get parent container information function
	//and have slot attribute type, inherited from TSlateContainedAttribute
	template<typename SlotType>
	class TWidgetSlotWithAttributeSupport : public TSlotBase<SlotType>, public SlateAttributePrivate::ISlateAttributeContainer
	{
	private:
		using Super = TSlotBase<SlotType>;

	protected:
		using TSlotBase<SlotType>::TSlotBase;//constructor

		/*
		 * a slate attribute is member variable of a FSlot
		 * @usage : TSlateSlotAttribute<int32_t> MyAttribute1; TSlateSlotAttribute<int32_t, TSlateAttributeComparePredicate<>> MyAttribute2;
		 */
		template<typename InObjectType, typename InComparePredicate = TSlateAttributeComparePredicate<>>
		struct TSlateSlotAttribute : public SlateAttributePrivate::TSlateContainedAttribute<InObjectType, SlateAttributePrivate::FSlateAttributeNoInvalidationReason, InComparePredicate>
		{
			//constructor
			using SlateAttributePrivate::TSlateContainedAttribute<InObjectType, SlateAttributePrivate::FSlateAttributeNoInvalidationReason, InComparePredicate>::TSlateContainedAttribute;
		};

	public:
		virtual ~TWidgetSlotWithAttributeSupport()
		{
			if(SWidget* widget_ptr = Super::get_owner_widget())//first to find FChildren owner, then find parent widget
			{
				//todo:remove container widget
				Remove_Container_Widget(*widget_ptr);
			}
		}

		void Construct(const FChildren& slot_owner, typename Super::FSlotArguments&& in_args)
		{
			//todo:check

			//note:call super TSlotBase<SlotType>'s construct function
			Super::Construct(slot_owner, std::move(in_args));
		}

		SWidget& Get_Container_Widget() const override
		{
			SWidget* widget_ptr = TSlotBase<SlotType>::get_owner_widget();//get parent widget

			return *widget_ptr;
		}

		DoDo::DoDoUtf8String Get_Container_Name() const override
		{
			//todo:FChildren is incomplete
			return FSlotBase::get_owner()->get_name();//return parent widget's debug name
		}

		//note:return position at parent container widget
		uint32_t Get_Container_Sort_Order() const override
		{
			const int32_t num_slot = FSlotBase::get_owner()->num_slot();//get parent widget's(container widget)
			for(int32_t index = 0; index < num_slot; ++index)
			{
				if(&FSlotBase::get_owner()->get_slot_at(index) == this)
				{
					return index;
				}
			}
		}
	};
}