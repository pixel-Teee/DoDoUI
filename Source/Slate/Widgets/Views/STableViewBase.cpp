#include "PreCompileHeader.h"

#include "STableViewBase.h"

#include "SListPanel.h"

#include "SlateCore/Styling/SlateTypes.h"//FScrollBarStyle depends on it

#include "SlateCore/Widgets/SBoxPanel.h"//SHorizontalBox

#include "Slate/Widgets/Layout/SScrollBar.h"

#include "ITableRow.h"

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
	void STableViewBase::clear_widgets()
	{
		m_items_panel->clear_items();
	}
	void STableViewBase::append_widget(const std::shared_ptr<ITableRow>& widget_to_append)
	{
		m_items_panel->add_slot()
		[
			widget_to_append->as_widget()
		];
	}
	void STableViewBase::insert_widget(const std::shared_ptr<ITableRow>& widget_to_insert)
	{
		m_items_panel->add_slot(0)
		[
			widget_to_insert->as_widget()
		];
	}
	int32_t STableViewBase::get_num_items_per_line() const
	{
		return 1;
	}
	void STableViewBase::Tick(const FGeometry& allotted_geometry, const double in_current_time, const float in_delta_time)
	{
		if (m_items_panel)
		{
			FGeometry panel_geometry = find_child_geometry(allotted_geometry, m_items_panel);

			if (m_panel_geometry_try_last_tick.get_local_size() != panel_geometry.get_local_size())
			{
				m_panel_geometry_try_last_tick = panel_geometry;

				const int32_t num_items_per_line = get_num_items_per_line();

				const EScrollIntoViewResult scroll_into_view_result = scroll_into_view(panel_geometry);

				double target_scroll_offset = get_target_scroll_offset();

				m_current_scroll_offset = target_scroll_offset;

				const FReGenerateResults re_generate_results = re_generate_items(panel_geometry);
				//m_last_generate_results = 
				
				//todo:add more logic
			}
		}
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
	double STableViewBase::get_target_scroll_offset() const
	{
		return m_desired_scroll_offset;
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