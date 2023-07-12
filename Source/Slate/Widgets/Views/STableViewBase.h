#pragma once

#include "Slate/Framework/Layout/IScrollableWidget.h"//IScrollableWidget depends on it

#include "SlateCore/Widgets/SCompoundWidget.h"//SCompoundWidget depends on it

#include "Slate/Framework/Views/ITypedTableView.h"//ETableViewMode::Type depends on it

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

	public:
		//IScrollableWidget interface
		virtual glm::vec2 get_scroll_distance() override;
		virtual glm::vec2 get_scroll_distance_remaining() override;
		virtual std::shared_ptr<SWidget> get_scroll_widget() override;
	protected:

		STableViewBase(ETableViewMode::Type in_table_view_mode);

		/*is this list backing a tree or just a standalone list*/
		const ETableViewMode::Type m_table_view_mode;

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
	};
}