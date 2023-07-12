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

		/*@return the uniform desired item dimensions used when arranging children*/
		FTableViewDimensions get_desired_item_dimensions() const;

		/*@return the horizontal padding applied to each tile item*/
		float get_item_padding(const FGeometry& allotted_geometry, const EListItemAlignment list_item_alignment) const;

		/*@return the horizontal padding applied to all the items on a line*/
		float get_line_padding(const FGeometry& allotted_geometry, const int32_t line_start_index) const;

		/*@return the uniform item width used when arraning children*/
		FTableViewDimensions get_item_size(const FGeometry& allotted_geometry, const EListItemAlignment list_item_alignment) const;

		FTableViewDimensions get_item_size(const FGeometry& allotted_geometry) const;
	protected:
		/*@return true if this panel should arrange items as tiles placed alongside one another in each line*/
		bool should_arrange_as_tiles() const;

	public:
		//swidget interface
		virtual void On_Arrange_Children(const FGeometry& allotted_geometry, FArrangedChildren& arranged_children) const override;
		virtual glm::vec2 Compute_Desired_Size(float) const override;
		virtual FChildren* Get_Children() override;
		//swidget interface

		virtual void On_Arrange_Children(const FGeometry& allotted_geometry, FArrangedChildren& arranged_children) const override;

	protected:
		/*the children being arranged by this panel*/
		TPanelChildren<FSlot> m_children;

		/*the uniform item width used to arrange the children, only relevant for tile views*/
		TAttribute<float> m_item_width;

		/*the uniform item height used to arrange the children*/
		TAttribute<float> m_item_height;

		/*total number of items that the tree wants to visualize*/
		TAttribute<int32_t> m_num_desired_items;

		/*
		* the offset of the view area from the top of the list in item heights
		* translate to physical units based on first line in list
		*/
		float m_first_line_scroll_offset = 0.0f;

		/*amount scrolled past beginning/end of list in slate units*/
		float m_over_scroll_amount = 0.0f;

		/*how should be horizontally aligned? only relevant for tile views*/
		TAttribute<EListItemAlignment> m_item_alignment;

		/*overall orientation of the list for layout and scrolling, only relevant for tile views*/
		EOrientation m_orientation;

		/*the preferred number of lines that this widget should have orthogonal to the scroll axis, only relevant for tile views*/
		int32_t m_preferred_num_lines = 1;
	};
}