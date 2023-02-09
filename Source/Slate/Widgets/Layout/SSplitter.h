#pragma once

#include "SlateCore/Widgets/SPanel.h"

#include "SlateCore/SlotBase.h"//FSlot depends on it

#include "SlateCore/Widgets/DeclarativeSyntaxSupport.h"

namespace DoDo {
	/*
	* SSplitter divides its allocated area into N segments, where N is the number of children it has
	* it allows the users to resize the children along the splitters axis, that is, horizontally or vertically
	*/
	class SSplitter : public SPanel
	{
	public:
		/*how should a child's size be determined*/
		enum ESizeRule
		{
			/*get the desired size() of the content*/
			SizeToContent,

			/*use a fraction of the parent's size*/
			FractionOfParent
		};

		using FOnSlotResized = Delegate<void(float)>;//the new size coefficient of the slot

		using FOnGetMaxSlotSize = Delegate<glm::vec2(int32_t)>;

	public:

		class FSlot : public TSlotBase<FSlot>
		{
		public:
			FSlot()
				: TSlotBase<FSlot>()
				, m_sizing_rule(FractionOfParent)
				, m_size_value(1)
				, m_min_size_value(0)
			{

			}

			SLATE_SLOT_BEGIN_ARGS(FSlot, TSlotBase<FSlot>)
				/*the size rule used by the slot*/
				SLATE_ATTRIBUTE(ESizeRule, SizeRule)
				/*when the rule size is set to FractionOfParent, the size of the slot is the value percentage of it's parent size*/
				SLATE_ATTRIBUTE(float, Value)
				/*minimum slot size when resizing*/
				SLATE_ATTRIBUTE(float, MinSize)
				/*can the slot be resize by the user*/
				SLATE_ARGUMENT(std::optional<bool>, Resizable)
				/*callback when the slot is resized*/
				SLATE_EVENT(FOnSlotResized, OnSlotResized)
				SLATE_SLOT_END_ARGS()

			void Construct(const FChildren& slot_owner, FSlotArguments&& in_args);

		public:
			TAttribute<ESizeRule> m_sizing_rule;

			TAttribute<float> m_size_value;

			TAttribute<float> m_min_size_value;

			FOnSlotResized m_on_slot_resized_handler;

			std::optional<bool> m_b_is_resizable;
		};
	};
}