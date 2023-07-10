#pragma once

#include "SlateCore/Widgets/SPanel.h"

#include "SlateCore/SlotBase.h"//TSlotBase depends on it

#include "Slate/Widgets/Views/STableViewBase.h"

namespace DoDo {
	/*
	* a really simple panel that arranges its children in a vertical list with no spacing
	* items in this panel have a uniform height
	* also supports offsetting its items vertically
	*/
	class SListPanel : public SPanel
	{
	public:
		/*a ListPanel slot is very simple - it just stores a widget*/
		class FSlot : public TSlotBase<FSlot>
		{
		public:
			FSlot()
			: TSlotBase<FSlot>()
			{}
		};

		SLATE_BEGIN_ARGS(SListPanel)
			: _ItemWidth(0)
			, _ItemHeight(16)
			, _NumDesiredItems(0)
			, _ItemAlignment(EListItemAlignment::EvenlyDistributed)
			, _ListOrientation(Orient_Vertical)
		{}
			SLATE_SLOT_ARGUMENT(FSlot, Slots)
			SLATE_ATTRIBUTE(float, ItemWidth)
			SLATE_ATTRIBUTE(float, ItemHeight)
			SLATE_ATTRIBUTE(int32_t, NumDesiredItems)
			SLATE_ATTRIBUTE(EListItemAlignment, ItemAlignment)
			SLATE_ARGUMENT(EOrientation, ListOrientation)
		SLATE_END_ARGS()

		SListPanel();

		/*make a new ListPanel::Slot*/
		static FSlot::FSlotArguments Slot();

		/*
		* construct the widget
		* 
		* @param InArgs a declaration from which to construct the widget
		*/
		void Construct(const FArguments& in_args);

	public:


	protected:
		/*the uniform item width used to arrange the children, only relevant for tile views*/
		TAttribute<float> m_item_width;

		/*the uniform item height used to arrange the children*/
		TAttribute<float> m_item_height;

		/*total number of items that the tree wants to visualize*/
		TAttribute<int32_t> m_num_desired_items;
	};
}