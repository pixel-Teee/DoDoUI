#include "PreCompileHeader.h"

#include "STableViewBase.h"

#include "SListPanel.h"

#include "SlateCore/Styling/SlateTypes.h"//FScrollBarStyle depends on it

#include "SlateCore/Widgets/SBoxPanel.h"//SHorizontalBox

#include "Slate/Widgets/Layout/SScrollBar.h"

#include "ITableRow.h"

#include "Slate/Widgets/Layout/SBox.h"//SBox

#include "Slate/Framework/Views/SHeaderRow.h"

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
						.ListOrientation(m_orientation);//todo:add clipping

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
			m_scroll_bar = SNew(SScrollBar);

			const FOptionalSize scroll_bar_size(16.0f);

			if (m_orientation == Orient_Vertical)
			{
				list_and_scroll_bar = SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.fill_width(1)
					[
						m_items_panel
					]
					+ SHorizontalBox::Slot()
					.auto_width()
					[
						SNew(SBox)
						.WidthOverride(scroll_bar_size)
						[
							m_scroll_bar
						]
					];
			}
			else
			{
				list_and_scroll_bar = SNew(SVerticalBox)
					+ SVerticalBox::Slot()
					.fill_height(1)
					[
						m_items_panel
					]
					+ SVerticalBox::Slot()
					.auto_height()
					[
						SNew(SBox)
						.HeightOverride(scroll_bar_size)
						[
							m_scroll_bar
						]
					];
			}
		}

		if (in_header_row)
		{
			//only associate the scrollbar if we created it
			if (!in_scroll_bar)
			{

			}

			this->m_child_slot
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.auto_height()
				[
					in_header_row
				]
				+ SVerticalBox::Slot()
				.fill_height(1)
				[
					list_and_scroll_bar
				]
			];
		}
		else
		{
			//check header row
			this->m_child_slot
			[
				list_and_scroll_bar
			];
		}	
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
	float STableViewBase::get_first_line_scroll_offset() const
	{
		const double first_line_scroll_offset = m_current_scroll_offset / get_num_items_per_line();

		return first_line_scroll_offset - (int64_t)first_line_scroll_offset;
	}
	void STableViewBase::set_scroll_offset(const float in_scroll_offset)
	{
		const float in_validate_offset = std::max(0.0f, in_scroll_offset);
		if (m_desired_scroll_offset != in_validate_offset)
		{
			m_desired_scroll_offset = in_validate_offset;

			//todo:call on table view scrolled

			//todo:add request layout refresh
		}
	}
	void STableViewBase::Tick(const FGeometry& allotted_geometry, const double in_current_time, const float in_delta_time)
	{
		if (m_items_panel)
		{
			FGeometry panel_geometry = find_child_geometry(allotted_geometry, m_items_panel);

			if (m_b_items_need_refresh || m_panel_geometry_try_last_tick.get_local_size() != panel_geometry.get_local_size())
			{
				m_panel_geometry_try_last_tick = panel_geometry;

				const int32_t num_items_per_line = get_num_items_per_line();

				const EScrollIntoViewResult scroll_into_view_result = scroll_into_view(panel_geometry);

				double target_scroll_offset = get_target_scroll_offset();//add animation logic

				m_current_scroll_offset = target_scroll_offset;

				const FReGenerateResults re_generate_results = re_generate_items(panel_geometry);

				m_last_generate_results = re_generate_results;

				const int32_t num_items_being_observed = get_num_items_being_observed();
				const int32_t num_item_lines = num_items_being_observed / num_items_per_line;

				const double initial_desired_offset = m_desired_scroll_offset;
				const bool b_enough_room_for_all_items = re_generate_results.m_exact_num_lines_on_screen >= num_item_lines;

				if (b_enough_room_for_all_items)
				{
					//we can show all the items, so make sure there is no scrolling
					set_scroll_offset(0.0f);
					m_current_scroll_offset = target_scroll_offset = m_desired_scroll_offset;
				}
				else if (re_generate_results.m_b_generated_past_last_item)
				{
					set_scroll_offset(std::max(0.0, re_generate_results.m_new_scroll_offset));
					m_current_scroll_offset = target_scroll_offset = m_desired_scroll_offset;
				}
				
				//todo:add more logic
				m_items_panel->set_first_line_scroll_offset(get_first_line_scroll_offset());

				if (m_allow_over_scroll == EAllowOverscroll::Yes)
				{
					//const float overscroll_amount = m_over

					//todo:add overscroll amount
				}

				//todo:add update selection set

				//todo:update scroll bar
				if (num_items_being_observed > 0)
				{
					if (re_generate_results.m_exact_num_lines_on_screen < 1.0f)
					{
						//we are be observing a single view row which is larger than the available visible area, so we should calculate thumb size based on that
						const double visible_size_fraction = allotted_geometry.get_local_size().y / re_generate_results.m_length_of_generated_items;
						const double thumb_size_fraction = std::min(visible_size_fraction, 1.0);
						const double offset_fraction = m_current_scroll_offset / num_items_being_observed;
						m_scroll_bar->set_state(offset_fraction, thumb_size_fraction);
					}
					else
					{
						//the thumb size is whatever fraction of the items we are currently seeing (including partially seen items)
						//e.g. if we are seeing 0.5 of the first generated widget and 0.75 of the last widget, that's 1.25 widgets
						const double thumb_size_fraction = re_generate_results.m_exact_num_lines_on_screen / num_item_lines;
						const double offset_fraction = m_current_scroll_offset / num_items_being_observed;
						m_scroll_bar->set_state(offset_fraction, thumb_size_fraction);
					}
				}
				else
				{
					const double thumb_size_fraction = 1;
					const double offset_fraction = 0;
					m_scroll_bar->set_state(offset_fraction, thumb_size_fraction);
				}
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
		, m_allow_over_scroll(EAllowOverscroll::Yes)
		, m_last_generate_results(0, 0, 0, false)
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