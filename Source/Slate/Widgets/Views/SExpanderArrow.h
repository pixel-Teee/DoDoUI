#pragma once

#include "SlateCore/Widgets/SCompoundWidget.h"

#include "SlateCore/Styling/CoreStyle.h"//FCoreStyle depends on it
#include "Slate/Widgets/Input/SButton.h"//SButton

namespace DoDo {
	class ITableRow;
	/*
	* expander arrow and indetation component that can be placed in a table row
	* of a tree view, intended for use by TMultiColumnRow in TreeViews
	*/
	class SExpanderArrow : public SCompoundWidget
	{
		SLATE_DECLARE_WIDGET(SExpanderArrow, SCompoundWidget)

	public:
		SLATE_BEGIN_ARGS(SExpanderArrow)
			: _StyleSet(&FCoreStyle::get())
			, _IndentAmount(10)
			, _BaseIndentLevel(0)
			, _ShouldDrawWires(false)
		{}
			SLATE_ARGUMENT(const ISlateStyle*, StyleSet)
			/*how many slate units to indent for every level of the tree*/
			SLATE_ATTRIBUTE(float, IndentAmount)
			/*the level that the root of the tree should start(e.g. 2 will shift the whole tree over by 'IndentAmount * 2')*/
			SLATE_ATTRIBUTE(int32_t, BaseIndentLevel)
			/*whether to draw the wires that visually reinforce the tree hierarchy*/
			SLATE_ATTRIBUTE(bool, ShouldDrawWires)
		SLATE_END_ARGS()

		SExpanderArrow();

		void Construct(const FArguments& in_args, const std::shared_ptr<ITableRow>& table_row);

	protected:
		virtual int32_t On_Paint(const FPaintArgs& args, const FGeometry& allotted_geometry, const FSlateRect& my_culling_rect, FSlateWindowElementList& out_draw_elements, int32_t layer_id, const FWidgetStyle& in_widget_style, bool b_parent_enabled) const override;

		/*@return visible when has children, invisible otherwise*/
		EVisibility get_expander_visibility() const;

		/*@return the margin corresponding to how far this item is indented*/
		FMargin get_expander_padding() const;

		/*invoked when the expanded button is clicked (toggle item expansion)*/
		FReply on_arrow_clicked();

		/*@return the name of an image that should be shown as the expander arrow*/
		const FSlateBrush* get_expander_image() const;

		std::weak_ptr<ITableRow> m_owner_row_ptr;

		/*a reference to the expander button*/
		std::shared_ptr<SButton> m_expander_arrow;

		/*the slate style to use*/
		const ISlateStyle* m_style_set;

		/*the amount of space to indent at each level*/
		TAttribute<float> m_indent_amount;

		/*the level in the tree that begins the indention amount*/
		TAttribute<int32_t> m_base_indent_level;

		/*whether to draw the wires that visually reinforce the tree hierarchy*/
		TSlateAttribute<bool> m_b_show_draw_wires;
	};
}