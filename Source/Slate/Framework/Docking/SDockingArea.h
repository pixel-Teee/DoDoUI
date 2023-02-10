#pragma once

#include "SDockingSplitter.h"

namespace DoDo
{
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
	};
}