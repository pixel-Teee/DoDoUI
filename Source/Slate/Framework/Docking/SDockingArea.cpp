#include <PreCompileHeader.h>

#include "SDockingArea.h"

#include "SlateCore/Widgets/SOverlay.h"

#include "SlateCore/Widgets/SBoxPanel.h"

namespace DoDo
{
	void SDockingArea::Construct(const FArguments& in_args, const std::shared_ptr<FTabManager>& in_tab_manager, const std::shared_ptr<FTabManager::FArea>& persistent_node)
	{
		m_my_tab_manager = in_tab_manager;

		//todo:implement on dock area created

		//in dock splitter mode we just act as a thin shell around a splitter widget
		this->m_child_slot
		[
			SNew(SOverlay)
			+ SOverlay::Slot()
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.auto_width()//the widget's desired size will be used as the space required
				[
					SAssignNew(m_splitter, SSplitter)
					.Orientation(persistent_node->get_orientation())
				]
			]
		];

		//if the owner window is set and bManageParentWindow is true, this docknode will close the window then its last tab is removed
		if (in_args._ParentWindow)
		{
			set_parent_window(in_args._ParentWindow);
		}
	}

	std::shared_ptr<FTabManager> SDockingArea::get_tab_manager() const
	{
		return m_my_tab_manager.lock();
	}
	std::shared_ptr<SWindow> SDockingArea::get_parent_window() const
	{
		return m_parent_window_ptr.expired() ? m_parent_window_ptr.lock() : std::shared_ptr<SWindow>();
	}
	void SDockingArea::set_parent_window(const std::shared_ptr<SWindow>& new_parent_window)
	{
		//todo:implement this function

		m_parent_window_ptr = new_parent_window;
	}
}
