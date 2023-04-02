#include <PreCompileHeader.h>

#include "SDockingArea.h"

#include "SlateCore/Widgets/SOverlay.h"

#include "SlateCore/Widgets/SBoxPanel.h"

#include "SlateCore/Application/SlateApplicationBase.h"

#include "Application/Application.h"

#include "SlateCore/Widgets/SNullWidget.h"

#include "SlateCore/Widgets/SWindow.h"

#include "Slate/Framework/Docking/SDockingTabStack.h"

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
			.Visibility(EVisibility::SelfHitTestInvisible)
			+ SOverlay::Slot()
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				//.fill_width(1.0f)//the widget's desired size will be used as the space required
				[
					SAssignNew(m_splitter, SSplitter)
					.Orientation(persistent_node->get_orientation())
				]
			]
			+ SOverlay::Slot()
			//house the minimize, maximize, restore, and icon
			//Visibility(EVisibility::SelfHitTestInvisible)
			.Expose(m_window_controls_area)
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Top)
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

		//event though we don't manage the parent window's lifetime, we are still responsible for making its window chrome
		{
			std::shared_ptr<IWindowTitleBar> title_bar;
		
			FWindowTitleBarArgs args(new_parent_window);
			args.m_center_content = SNullWidget::NullWidget;
			args.m_center_content_alignment = HAlign_Fill;
		
			std::shared_ptr<SWidget> title_bar_widget = Application::get().make_window_title_bar(args, title_bar);
		
			(*m_window_controls_area)
			[
				title_bar_widget
			];
		
			new_parent_window->set_title_bar(title_bar);
		}

		m_parent_window_ptr = new_parent_window;
	}
	void SDockingArea::add_side_bar_tabs_from_restored_layout(const FSidebarTabLists& side_bar_tabs)
	{

	}
	void SDockingArea::clean_up(ELayoutModification removal_method)
	{

		//in some cases a dock area will control the window
		//and we need to move some of the tabs out of the way to make room for window chrome
		update_window_chrome_and_side_bar();
	}
	void SDockingArea::update_window_chrome_and_side_bar()
	{

		//reserve some space for the minimize, restore, and close controls
		std::shared_ptr<SDockingTabStack> window_control_housing = this->find_tab_stack_to_house_window_controls();
		window_control_housing->reserve_space_for_window_chrome(SDockingTabStack::EChromeElement::Controls, false, false);

		//reserve some space for the app icons
		std::shared_ptr<SDockingTabStack> icon_housing = this->find_tab_stack_to_house_window_icon();
		icon_housing->reserve_space_for_window_chrome(SDockingTabStack::EChromeElement::Icon, true, true);//todo:fix me

	}

}
