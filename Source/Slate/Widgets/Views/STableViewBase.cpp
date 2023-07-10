#include "PreCompileHeader.h"

#include "STableViewBase.h"

namespace DoDo {
	void STableViewBase::ConstructChildren(const TAttribute<float>& in_item_widget, const TAttribute<float>& in_item_height, const TAttribute<EListItemAlignment>& in_item_alignment, const std::shared_ptr<SHeaderRow>& in_header_row, const std::shared_ptr<SScrollBar>& in_scroll_bar, EOrientation in_scroll_orientation, const FOnTableViewScrolled& in_on_table_view_scrolled)
	{
		m_header_row = in_header_row;

		m_on_table_view_scrolled = in_on_table_view_scrolled;

		m_orientation = in_header_row ? Orient_Vertical : in_scroll_orientation;


	}
}