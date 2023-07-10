#pragma once

#include "Slate/Framework/Layout/IScrollableWidget.h"//IScrollableWidget depends on it

#include "SlateCore/Widgets/SCompoundWidget.h"//SCompoundWidget depends on it

namespace DoDo {
	class SHeaderRow;
	class SScrollBar;
	class SListPanel;
	/*if the list panel is arranging items as tiles, this enum dictates how the items should be aligned (basically, where any extra space is placed)*/
	enum class EListItemAlignment : uint8_t
	{
		/*items are distributed evenly along the line (any extra space is added as padding between the items)*/
		EvenlyDistributed,

		/*items are distributed evenly along the line (any extra space is used to scale up the size of the item proportionally)*/
		EvenlySize,

		/*items are distributed evenly along the line (any extra space is used to scale up the width of the items proportionally)*/
		EvenlyWide,

		/*items are left aligned on the line (any extra space is added to the right of the items)*/
		LeftAligned,

		/*items are right aligned on the line (any extra space is added to the left of the items)*/
		RightAligned
	};
	
	DECLARE_DELEGATE_OneParam(
		FOnTableViewScrolled,
		double
	);//scroll offset from the beginning of the list in items
	/*
	* Contains ListView functionality that does not depend on the type of data being observed by the ListView
	*/
	class STableViewBase : public SCompoundWidget, public IScrollableWidget
	{
	public:
		/*create the child widgets that comprise the list*/
		void ConstructChildren(const TAttribute<float>& in_item_widget, const TAttribute<float>& in_item_height, const TAttribute<EListItemAlignment>& in_item_alignment,
			const std::shared_ptr<SHeaderRow>& in_header_row, const std::shared_ptr<SScrollBar>& in_scroll_bar, EOrientation in_scroll_orientation, const FOnTableViewScrolled& in_on_table_view_scrolled); //todo:add FScrollBarStyle

	protected:
		/*column headers that describe which columns this list shows*/
		std::shared_ptr<SHeaderRow> m_header_row;

		/*the panel which holds the visible widgets in the list*/
		std::shared_ptr<SListPanel> m_items_panel;

		/*delegate to call when the table view is scrolled*/
		FOnTableViewScrolled m_on_table_view_scrolled;

		/*the layout and scroll orientation of the list*/
		EOrientation m_orientation = Orient_Vertical;
	};
}