#pragma once

#include "SlateCore/Widgets/SPanel.h"

#include "SlateCore/Layout/Children.h"//TSlotlessChildren depends on it

#include "SlateCore/Widgets/DeclarativeSyntaxSupport.h"

#include "Slate/Widgets/Docking/SDockTab.h"

namespace DoDo {
	struct FDockingConstants
	{
		static const glm::vec2 max_minor_tab_size;
		static const glm::vec2 max_major_tab_size;
		static const glm::vec2 get_max_tab_size_for(ETabRole tab_role);
	};

	class FDragDropEvent;
	class SDockingTabStack;
	struct FSlateBrush;
	//class SDockTab;
	/*
	* tabwell is a panel that shows dockable tabs
	* tabs can be re-arranged and dragged out of the tab stack
	*/
	class SDockingTabWell : public SPanel
	{
	public:
		SLATE_BEGIN_ARGS(SDockingTabWell)
			: _ParentStackNode(std::shared_ptr<SDockingTabStack>(nullptr))
			{}

		SLATE_ATTRIBUTE(std::shared_ptr<SDockingTabStack>, ParentStackNode)
		SLATE_END_ARGS()

		SDockingTabWell();

		void Construct(const FArguments& in_args);

		/*@return how many tabs there are*/
		int32_t get_num_tabs() const;

		/*gets the currently active tab (or the currently dragged tab), or a null pointer if no tab is active*/
		std::shared_ptr<SDockTab> get_foreground_tab() const;

		void refresh_parent_cotent();

		/*gets the parent dockable tab stack this tab well belong to*/
		std::shared_ptr<SDockingArea> get_dock_area();

		/*
		* add a new tab (InTab) to the TabWell at location (AtIndex)
		* 
		* @param InTab TheTab to add
		* @param AtIndex add at the index or at the end if INDEX_NONE(default)
		*/
		void add_tab(const std::shared_ptr<SDockTab>& in_tab, int32_t at_index = -1, bool b_keep_inactive = false);

		/*
		* activate the tab specified by tab to activate index
		*/
		void bring_tab_to_front(int32_t tab_index_to_active);

		/*activate the tab specified by tab to activate SDockTab*/
		void bring_tab_to_front(std::shared_ptr<SDockTab> tab_to_activate);

		glm::vec2 Compute_Child_Size(const FGeometry& allotted_geometry) const;

		//SWidget interface
		virtual void On_Arrange_Children(const FGeometry& allotted_geometry, FArrangedChildren& arranged_children) const override;
		virtual int32_t On_Paint(const FPaintArgs& args, const FGeometry& allotted_geometry, const FSlateRect& my_culling_rect, FSlateWindowElementList& out_draw_elements, int32_t layer_id, const FWidgetStyle& in_widget_style, bool b_parent_enabled) const override;
		virtual void On_Drag_Enter(const FGeometry& my_geometry, const FDragDropEvent& drag_drop_event) override;
		virtual void On_Drag_Leave(const FDragDropEvent& drag_drop_event) override;
		virtual FReply On_Drag_Over(const FGeometry& my_geometry, const FDragDropEvent& drag_drop_event) override;
		virtual FReply On_Drop(const FGeometry& my_geometry, const FDragDropEvent& drag_drop_event) override;
		virtual glm::vec2 Compute_Desired_Size(float) const override;
		virtual FChildren* Get_Children() override;
		virtual FReply On_Mouse_Button_On_Up(const FGeometry& my_geometry, const FPointerEvent& mouse_event) override;
		virtual FReply On_Mouse_Move(const FGeometry& my_geometry, const FPointerEvent& mouse_event) override;
		virtual EWindowZone::Type get_window_zone_override() const override;
		//SWidget interface

		/*
		 * a tab notifies us that the started dragging it, so we should begin re-arranging layout
		 *
		 * @param TabToStartDragging the tab that notified us that the user started dragging it
		 * @param TabGrabOffsetFraction the offset into the tab where the user grabbed it, as a fraction of the tab's size
		 * @param MouseEvent the mouse event that caused this dragging to start
		 *
		 * @return the dragdrop operation
		 */
		FReply start_dragging_tab(std::shared_ptr<SDockTab> tab_to_start_dragging, glm::vec2 tab_grab_offset_fraction, const FPointerEvent& mouse_event);

	private:
		/*
		 * compute the offset of the tab being dragged given:
		 *
		 * @param MyGeometry the tab well's geometry
		 * @param MouseEvent the mouse event that is effecting the drag
		 * @param tab grab offset fraction     how far into the tab the user grabbed it, as a faction of the tab's size
		 *
		 * @return the offset of the tab from the beginning of the tab well
		 */
		float compute_dragged_tab_offset(const FGeometry& my_geometry, const FPointerEvent& mouse_event, const glm::vec2& tab_grab_offset_fraction) const;

		/*
		* the user is dropping a tab on this tabwell, and the tabwell's geometry is currently my geometry,
		* @return the index of the slot into which the tab should go
		*/
		int32_t compute_child_drop_index(const FGeometry& my_geometry, const std::shared_ptr<SDockTab>& tab_being_dragged);
	private:
		/*the tabs in this tabwell*/
		TSlotlessChildren<SDockTab> m_tabs;

		/*a pointer to the dock node that owns this tabwell*/
		std::weak_ptr<SDockingTabStack> m_parent_tab_stack_ptr;

		/*the brush displayed between tabs*/
		const FSlateBrush* m_separator_brush;

		/*the tab being dragged through the tab well, if there is one*/
		std::shared_ptr<SDockTab> m_tab_being_dragged_ptr;

		/*the offset of the tab being dragged through this panel*/
		float m_child_being_dragged_offset;

		/*where the user grabbed the tab as a fraction of the tab's size*/
		glm::vec2 m_tab_grab_offset_fraction;

		/*the index of the tab that is in the foreground right now, index_none if either none are active or a tab is being dragged throgh*/
		int32_t m_foreground_tab_index;
	};
}