#pragma once

#include "SDockingSplitter.h"

#include "SlateCore/Widgets/SOverlay.h"//SOverlay::FOverlaySlot depends on it

namespace DoDo
{
	class SDockTab;

	/*
	* list of tabs that should be in each sidebar
	*/
	struct FSidebarTabLists
	{
		std::vector<std::shared_ptr<SDockTab>> m_left_side_bar_tabs;
		std::vector<std::shared_ptr<SDockTab>> m_right_side_bar_tabs;
	};
	/*
	 * represents the root node in a hierarchy of DockNodes
	 */
	class SDockingArea : public SDockingSplitter
	{
	public:
		SLATE_BEGIN_ARGS(SDockingArea)
			: _ParentWindow()
			, _ShouldManageParentWindow(true)
			, _InitialContent()
			{
				//todo:modify visibility
			}
			/*
			 * the window whose content area this is directly embedded within, by default, should manage parent window is
			 * set to true, which means the dock area will also destroy the window when the last tab goes away
			 * assigning a parent window also allows the docking area to embed title area widgets (minimize, maximize, etc) into its content area
			 */
			SLATE_ARGUMENT(std::shared_ptr<SWindow>, ParentWindow)

			/*true if this docking area should close the parent window when the last tab in this docking area goes away*/
			SLATE_ARGUMENT(bool, ShouldManageParentWindow)

			/*what to put into the DockArea initially, usually a TabStack, so that some tabs can be added to it*/
			SLATE_ARGUMENT(std::shared_ptr<SDockingNode>, InitialContent)
		SLATE_END_ARGS()

		void Construct(const FArguments& in_args, const std::shared_ptr<FTabManager>& in_tab_manager, const std::shared_ptr<FTabManager::FArea>& persistent_node);

		/*returns this dock area*/
		virtual std::shared_ptr<SDockingArea> get_dock_area() override;

		virtual std::shared_ptr<FTabManager::FLayoutNode> gather_persistent_layout() const override;

		std::shared_ptr<FTabManager> get_tab_manager() const;

		/*returns the window that this dock area resides in directly and also manages*/
		std::shared_ptr<SWindow> get_parent_window() const;

		void set_parent_window(const std::shared_ptr<SWindow>& new_parent_window);

		/*
		* adds all tabs back to a sidebar that were saved in a sidebar from a previous session
		*/
		void add_side_bar_tabs_from_restored_layout(const FSidebarTabLists& side_bar_tabs);

		/*
		* removes redundant stack and splitters, collapses any widgets that any no longer showing live content
		*/
		void clean_up(ELayoutModification removal_method);

		/*
		* if this dock area controls a window, then we need
		* to reserve some room in the upper left and upper right tab wells
		* so that there is no overlap with the window chrome
		* 
		* we also update the sidebar to account for major tabs, docking areas for major tabs do not have a sidebar
		*/
		void update_window_chrome_and_side_bar();

		void on_tab_found_new_home(const std::shared_ptr<SDockTab>& relocated_tab, const std::shared_ptr<SWindow>& new_owner_window);

	protected:
		virtual SDockingNode::ECleanupRetVal clean_up_nodes() override;

	private:
		EVisibility target_cross_visibility() const;

		EVisibility target_cross_center_visibility() const;
		/*the tab manager that controls this dock area*/
		std::weak_ptr<FTabManager> m_my_tab_manager;

		/*
		* we don't want to waste a lot of space for the minimize, restore close buttons and other windows controls
		* dock areas that manage a parent window will use this slot to house those controls
		*/
		SOverlay::FOverlaySlot* m_window_controls_area;

		/*the window this dock area is embedded within, if bIsManagingParentWindow is true, the dock area will also destroy the window when the last tab goes away*/
		std::weak_ptr<SWindow> m_parent_window_ptr;

		/*the overlay is visible when the user is dragging a tab over the dock area*/
		bool m_b_is_overlay_visible;

		/*the center target is visible when the overlay is visible and there are no live tabs*/
		bool m_b_is_center_target_visible;
	};
}