#pragma once

#include "SDockingSplitter.h"

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

		std::shared_ptr<FTabManager> get_tab_manager() const;

		/*returns the window that this dock area resides in directly and also manages*/
		std::shared_ptr<SWindow> get_parent_window() const;

	private:
		/*the tab manager that controls this dock area*/
		std::weak_ptr<FTabManager> m_my_tab_manager;

		/*the window this dock area is embedded within, if bIsManagingParentWindow is true, the dock area will also destroy the window when the last tab goes away*/
		std::weak_ptr<SWindow> m_parent_window_ptr;
	};
}