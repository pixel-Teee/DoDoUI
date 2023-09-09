#pragma once

#include "Slate/Framework/Layout/IScrollableWidget.h"//IScrollableWidget depends on it

#include "SlateCore/Widgets/SCompoundWidget.h"//SCompoundWidget depends on it

#include "Slate/Framework/Views/ITypedTableView.h"//ETableViewMode::Type depends on it

#include "Slate/Framework/Layout/Overscroll.h"//EAllowOverscroll depends on it

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
		RightAligned,

		/*items are center aligned on the line (any extra space is halved and added to the left of the items)*/
		CenterAligned,

		/*items are evenly stretched to distribute any extra space on the line*/
		Fill
	};
	
	DECLARE_DELEGATE_OneParam(
		FOnTableViewScrolled,
		double
	);//scroll offset from the beginning of the list in items

	/*abstracts away the need to distinguish between x or y when calculating table layout elements*/
	struct FTableViewDimensions
	{
		FTableViewDimensions(EOrientation in_orientation);

		FTableViewDimensions(EOrientation in_orientation, float x, float y);

		FTableViewDimensions(EOrientation in_orientation, const glm::vec2& size);

		glm::vec2 to_vector_2d() const
		{
			return m_orientation == Orient_Vertical ? glm::vec2(m_line_axis, m_scroll_axis) : glm::vec2(m_scroll_axis, m_line_axis);
		}

		FTableViewDimensions operator+(const FTableViewDimensions& other) const
		{
			return FTableViewDimensions(m_orientation, to_vector_2d() + other.to_vector_2d());
		}

		EOrientation m_orientation = Orient_Vertical;

		/*the dimension along the scrolling axis of the table view (y when oriented vertically, x when horizontal)*/
		float m_scroll_axis = 0.0f;

		/*the dimension orthognoal to the scroll axis, along which lines of items are created, only really relevant for tile views*/
		float m_line_axis = 0.0f;
	};

	struct FScrollBarStyle;
	class ITableRow;
	/*
	* Contains ListView functionality that does not depend on the type of data being observed by the ListView
	*/
	class STableViewBase : public SCompoundWidget, public IScrollableWidget
	{
	public:
		/*create the child widgets that comprise the list*/
		void ConstructChildren(const TAttribute<float>& in_item_width, const TAttribute<float>& in_item_height, const TAttribute<EListItemAlignment>& in_item_alignment,
			const std::shared_ptr<SHeaderRow>& in_header_row, const std::shared_ptr<SScrollBar>& in_scroll_bar, EOrientation in_scroll_orientation, const FOnTableViewScrolled& in_on_table_view_scrolled, const FScrollBarStyle* in_scrollbar_style = nullptr); //todo:add FScrollBarStyle

		/*@return how many items there are in the TArray being observed*/
		virtual int32_t get_num_items_being_observed() const = 0;

		/*is this list backing a tree or just a standalone list*/
		const ETableViewMode::Type m_table_view_mode;

		/*
		* remove all the widgets from the view
		*/
		void clear_widgets();

		/*information about the outcome of the widget regenerate pass*/
		struct FReGenerateResults
		{
			FReGenerateResults(double in_new_scroll_offset, double in_length_generated, double in_items_on_screen, bool at_end_of_list)
				: m_new_scroll_offset(in_new_scroll_offset)
				, m_length_of_generated_items(in_length_generated)
				, m_exact_num_lines_on_screen(in_items_on_screen)
				, m_b_generated_past_last_item(at_end_of_list)
			{}

			/*the scroll offset that we actually use might not be what the user asked for*/
			double m_new_scroll_offset = 0.0;

			/*the total length along the scroll axis of the widgets that we have generated to represent the visible subset of the items*/
			double m_length_of_generated_items = 0.0;

			/*how many lines are fitting on the screen, including fractions*/
			double m_exact_num_lines_on_screen = 0.0;

			/*true when we have generated*/
			bool m_b_generated_past_last_item = false;
		};

		/*
		* update generate widgets for items as needed and clean up any widgets that are no longer needed
		* re-arrange the visible widget order as necessary
		*/
		virtual FReGenerateResults re_generate_items(const FGeometry& my_geometry) = 0;

		enum class EScrollIntoViewResult
		{
			/*the function scrolled an item (if set) into view (or the item was already in view)*/
			Success,

			/*the function did not have enough data to scroll the given item into view, so it should be deferred until the next tick*/
			Deferred,

			/*the function failed to scroll to the specified item*/
			Failure
		};

		/*
		* if there is a pending request to scroll an item into view, do so
		* 
		* @param ListViewGeometry the geometry of the list view, can be useful for centering the item
		*/
		virtual EScrollIntoViewResult scroll_into_view(const FGeometry& list_view_geometry) = 0;

		/*add a widget to append to the bottom of the view*/
		void append_widget(const std::shared_ptr<ITableRow>& widget_to_append);

		/*insert widget to insert at the top of the view*/
		void insert_widget(const std::shared_ptr<ITableRow>& widget_to_insert);

		/*
		* get the number of items that can fit in the view along the line axis (orthogonal to the scroll axis) before creating a new line
		* default is 1, but may be more in subclasses (like STileView)
		*/
		virtual int32_t get_num_items_per_line() const;

		/*get the offset of the first list item*/
		virtual float get_first_line_scroll_offset() const;

		/*set the scroll offset of this view (in items)*/
		void set_scroll_offset(const float in_scroll_offset);
	public:
		//SWidget interface

		virtual void Tick(const FGeometry& allotted_geometry, const double in_current_time, const float in_delta_time) override;
	public:
		//IScrollableWidget interface
		virtual glm::vec2 get_scroll_distance() override;
		virtual glm::vec2 get_scroll_distance_remaining() override;
		virtual std::shared_ptr<SWidget> get_scroll_widget() override;
	protected:

		STableViewBase(ETableViewMode::Type in_table_view_mode);

		/*returns the "true" scroll offset where the list will ultimately settle (and may already be)*/
		double get_target_scroll_offset() const;

		/*information about the widgets we generated during the last regenerate pass*/
		FReGenerateResults m_last_generate_results;

		/*what the list's geometry was the last time a refresh occurred*/
		FGeometry m_panel_geometry_try_last_tick;

		double m_desired_scroll_offset = 0.;

		/*the currently dispalyed scroll offset from the beginning of the list in items*/
		double m_current_scroll_offset = 0.;

		/*column headers that describe which columns this list shows*/
		std::shared_ptr<SHeaderRow> m_header_row;

		/*the panel which holds the visible widgets in the list*/
		std::shared_ptr<SListPanel> m_items_panel;

		/*the scroll bar widget*/
		std::shared_ptr<SScrollBar> m_scroll_bar;

		/*delegate to call when the table view is scrolled*/
		FOnTableViewScrolled m_on_table_view_scrolled;

		/*the layout and scroll orientation of the list*/
		EOrientation m_orientation = Orient_Vertical;

	protected:
		//todo:add FOverscroll

		/*whether to permit overscroll on this list view*/
		EAllowOverscroll m_allow_over_scroll;

	private:
		/*when true, a refresh shoud occur the next tick*/
		bool m_b_items_need_refresh = false;
	};
}