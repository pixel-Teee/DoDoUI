#include "PreCompileHeader.h"

#include "STableViewBase.h"

#include "SListPanel.h"

#include "SlateCore/Styling/SlateTypes.h"//FScrollBarStyle depends on it

#include "SlateCore/Widgets/SBoxPanel.h"//SHorizontalBox

#include "Slate/Widgets/Layout/SScrollBar.h"

namespace DoDo {
	void STableViewBase::ConstructChildren(const TAttribute<float>& in_item_width, const TAttribute<float>& in_item_height, const TAttribute<EListItemAlignment>& in_item_alignment, const std::shared_ptr<SHeaderRow>& in_header_row, const std::shared_ptr<SScrollBar>& in_scroll_bar, EOrientation in_scroll_orientation, const FOnTableViewScrolled& in_on_table_view_scrolled,
		const FScrollBarStyle* in_scroll_bar_style)
	{
		m_header_row = in_header_row;

		m_on_table_view_scrolled = in_on_table_view_scrolled;

		m_orientation = in_header_row ? Orient_Vertical : in_scroll_orientation;

		m_items_panel = SNew(SListPanel)
						.ItemWidth(in_item_width)
						.ItemHeight(in_item_height)
						.NumDesiredItems(this, &STableViewBase::get_num_items_being_observed)
						.ItemAlignment(in_item_alignment)
						.ListOrientation(m_orientation);

		std::shared_ptr<SWidget> list_and_scroll_bar;
		if (in_scroll_bar)
		{
			//the user provided us with a scrollbar, we will rely on it
			m_scroll_bar = in_scroll_bar;
			//todo:add set on user scrolled

			list_and_scroll_bar = m_items_panel;
		}
		else
		{
			//m_scroll_bar = SNew(SScrollBar)

			if (m_orientation == Orient_Vertical)
			{
				list_and_scroll_bar = SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.fill_width(1)
					[
						m_items_panel
					];
			}
			else
			{
				list_and_scroll_bar = SNew(SVerticalBox)
					+ SVerticalBox::Slot()
					.fill_height(1)
					[
						m_items_panel
					];
			}
		}

		//check header row
		this->m_child_slot
		[
			list_and_scroll_bar
		];
	}
	glm::vec2 STableViewBase::get_scroll_distance()
	{
		return glm::vec2(0, m_scroll_bar->distance_from_top());
	}
	glm::vec2 STableViewBase::get_scroll_distance_remaining()
	{
		return glm::vec2(0, m_scroll_bar->distance_from_bottom());
	}
	std::shared_ptr<SWidget> STableViewBase::get_scroll_widget()
	{
		return shared_from_this();
	}
	STableViewBase::STableViewBase(ETableViewMode::Type in_table_view_mode)
		: m_table_view_mode(in_table_view_mode)
	{
	}
	FTableViewDimensions::FTableViewDimensions(EOrientation in_orientation)
		: m_orientation(in_orientation)
	{
	}
	FTableViewDimensions::FTableViewDimensions(EOrientation in_orientation, float x, float y)
		: FTableViewDimensions(in_orientation, glm::vec2(x, y))
	{
	}
	FTableViewDimensions::FTableViewDimensions(EOrientation in_orientation, const glm::vec2& size)
		: FTableViewDimensions(in_orientation)
	{
		if (in_orientation == Orient_Vertical)
		{
			m_line_axis = size.x;
			m_scroll_axis = size.y;
		}
		else
		{
			m_line_axis = size.y;
			m_scroll_axis = size.x;
		}
	}
}