#pragma once

#include "SWidget.h"

namespace DoDo
{
	/*
	 * a panel arranges it's child widgets on the screen
	 *
	 * each child widget should be stored in a slot, the slot describes how the individual child should be arranged with
	 * respect to its parent (i.e. the Panel) and it's peers widgets (i.e. the panel's other children)
	 * for a simple examples see StackPanel
	 */
	class SPanel : public SWidget
	{
	public:
		/*
		 * panels arrange their children in a space described by the AllottedGeometry parameter, the results of the arrangement
		 * should be returned by appending a FArrangedWidget pair for every child widget, see stack panel for an example
		 *
		 * @param AllottedGeometry the geometry allotted for this widget by it's parent
		 * @param ArrangedChildren the array to which to add the widget geometries that represent the arranged children
		 */
		virtual void On_Arrange_Children(const FGeometry& allotted_geometry, FArrangedChildren& arranged_children) const override = 0;

		/*
		 * all widgets must provide a way to access their children in a layout-agnostic way
		 * panels store their children in slots, which creates a dilemma, most panels
		 * can store their children in a TPanelChildren<Slot>, where the slot class
		 * provides layout information about the child it stores, in that case
		 * get children should simply return the TPanelChildren<Slot>, see stack panel for example
		 */
		virtual FChildren* Get_Children() override = 0;

	public:
		/*
		 * most panels no dot create widgets as part of their implementation, so
		 * they do not need to implement a construct()
		 */
		void Construct() {}

	public:

		virtual int32_t On_Paint(const FPaintArgs& args, const FGeometry& allotted_geometry, const FSlateRect& my_culling_rect, FSlateWindowElementList& out_draw_elements,
			int32_t layer_id, const FWidgetStyle& in_widget_style, bool b_parent_enabled) const override;

	protected:

		int32_t paint_arranged_children(const FPaintArgs& args, const FArrangedChildren& arranged_children, const FGeometry& allotted_geometry, const FSlateRect& my_culling_rect, FSlateWindowElementList& out_draw_elements, int32_t layer_id, const FWidgetStyle& in_widget_style, bool b_parent_enabled) const;

	protected:
		/*hidden default constructor*/
		SPanel(){}
	};
}
