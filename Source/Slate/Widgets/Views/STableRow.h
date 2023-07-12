#pragma once

#include "ITableRow.h"

#include "Slate/Widgets/Layout/SBorder.h"//SBoder depends on it

namespace DoDo {
	/*
	* the list view is populated by selectable widgets
	* a selectable widget is a way of the list view containing it (owner table) and holds arbitrary content (content)
	* a selectable works with its corresponding list view to provide selection functionality
	*/
	template<typename ItemType>
	class STableRow : public ITableRow, public SBorder
	{
	public:
		SLATE_BEGIN_ARGS(STableRow<ItemType>)
			: _ExpanderStyleSet(&FCoreStyle::get())
			{}
			SLATE_ARGUMENT(const ISlateStyle*, ExpanderStyleSet)
		SLATE_END_ARGS()
	};
}