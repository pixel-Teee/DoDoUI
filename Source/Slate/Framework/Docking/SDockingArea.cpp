#include <PreCompileHeader.h>

#include "SDockingArea.h"

#include "SlateCore/Widgets/SOverlay.h"

#include "SlateCore/Widgets/SBoxPanel.h"

#include "SlateCore/Application/SlateApplicationBase.h"

#include "Application/Application.h"

#include "SlateCore/Widgets/SNullWidget.h"

#include "SlateCore/Widgets/SWindow.h"

#include "Slate/Framework/Docking/SDockingTabStack.h"

#include "Core/Misc/Attribute.h"//TAttribute depends on it

#include "Slate/Framework/Docking/SDockingTarget.h"//SDockingNode depends on it

namespace DoDo
{
	void SDockingArea::Construct(const FArguments& in_args, const std::shared_ptr<FTabManager>& in_tab_manager, const std::shared_ptr<FTabManager::FArea>& persistent_node)
	{
		m_my_tab_manager = in_tab_manager;
		in_tab_manager->get_private_api().on_dock_area_created(std::static_pointer_cast<SDockingArea>(shared_from_this()));

		m_b_is_overlay_visible = false;

		m_b_is_center_target_visible = false;

		//todo:implement on dock area created

		const TAttribute<EVisibility> target_cross_visibility = TAttribute<EVisibility>(std::static_pointer_cast<SDockingArea>(shared_from_this()), &SDockingArea::target_cross_visibility);
		const TAttribute<EVisibility> target_cross_center_visibility = TAttribute<EVisibility>(std::static_pointer_cast<SDockingArea>(shared_from_this()), &SDockingArea::target_cross_center_visibility);

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

			+ SOverlay::Slot()
			.HAlign(HAlign_Left)
			.VAlign(VAlign_Fill)
			[
				SNew(SDockingTarget)
				.Visibility(target_cross_visibility)
				.OwnerNode(std::static_pointer_cast<SDockingNode>(shared_from_this()))
				.DockDirection(SDockingNode::LeftOf)
			]

			+ SOverlay::Slot()
			.HAlign(HAlign_Right)
			.VAlign(VAlign_Fill)
			[
				SNew(SDockingTarget)
				.Visibility(target_cross_visibility)
				.OwnerNode(std::static_pointer_cast<SDockingNode>(shared_from_this()))
				.DockDirection(SDockingNode::RightOf)
			]

			+ SOverlay::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Top)
			[
				SNew(SDockingTarget)
				.Visibility(target_cross_visibility)
				.OwnerNode(std::static_pointer_cast<SDockingNode>(shared_from_this()))
				.DockDirection(SDockingNode::Above)
			]

			+ SOverlay::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Bottom)
			[
				SNew(SDockingTarget)
				.Visibility(target_cross_visibility)
				.OwnerNode(std::static_pointer_cast<SDockingNode>(shared_from_this()))
				.DockDirection(SDockingNode::Below)
			]

			+ SOverlay::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Center)
			[
				SNew(SDockingTarget)
				.Visibility(target_cross_visibility)
				.OwnerNode(std::static_pointer_cast<SDockingNode>(shared_from_this()))
				.DockDirection(SDockingNode::Center)
			]
		];

		m_b_clenup_upon_tab_relocation = false;

		//if the owner window is set and bManageParentWindow is true, this docknode will close the window then its last tab is removed
		if (in_args._ParentWindow)
		{
			set_parent_window(in_args._ParentWindow);
		}

		//add initial content if it was provided, for example, stack node
		if (in_args._InitialContent)
		{
			add_child_node(in_args._InitialContent);
		}
	}

	std::shared_ptr<SDockingArea> SDockingArea::get_dock_area()
	{
		return std::static_pointer_cast<SDockingArea>(shared_from_this());
	}

	std::shared_ptr<FTabManager::FLayoutNode> SDockingArea::gather_persistent_layout() const
	{
		//assume that all the nodes were dragged out, and there's no meaningful layout data to be gathered
		bool b_have_layout_node = false;

		std::shared_ptr<FTabManager::FArea> persistent_node;

		std::shared_ptr<SWindow> parent_window = m_parent_window_ptr.lock();

		if (parent_window) //todo:add manager parent window bool variable check
		{
			//todo:fix me

			float dpi_scale = 1.0f;

			persistent_node = FTabManager::new_area(parent_window->get_size_in_screen().x, parent_window->get_size_in_screen().y);

			persistent_node->set_window(parent_window->get_position_in_screen(), false);
		}
		else
		{
			//an area without a window persists because it must be a primary area
			//those must always be restore, even if they are empty
			persistent_node = FTabManager::new_primary_area();
			b_have_layout_node = true;
		}

		persistent_node->set_orientation(this->get_orientation());

		for (int32_t child_index = 0; child_index < m_children.size(); ++child_index)
		{
			std::shared_ptr<FTabManager::FLayoutNode> persistent_child = m_children[child_index]->gather_persistent_layout();
			if (persistent_child)
			{
				b_have_layout_node = true;
				persistent_node->split(persistent_child);
			}
		}

		if (!b_have_layout_node)
		{
			persistent_node.reset();
		}

		return persistent_node;
	}

	std::shared_ptr<FTabManager> SDockingArea::get_tab_manager() const
	{
		return m_my_tab_manager.lock();
	}
	std::shared_ptr<SWindow> SDockingArea::get_parent_window() const
	{
		return m_parent_window_ptr.lock() ? m_parent_window_ptr.lock() : std::shared_ptr<SWindow>();
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
		const ECleanupRetVal cleanup_result = clean_up_nodes();//note:this will keep the dock hierarchy
		
		if (cleanup_result != VisibleTabsUnderNode)
		{
			m_b_is_center_target_visible = true;
		
			//we may have a window to manage
			std::shared_ptr<SWindow> parent_window = m_parent_window_ptr.lock();
		
			//todo:add manager parent window bool variable check
			if (parent_window)
			{
				if (removal_method == TabRemoval_Closed)
				{
					m_my_tab_manager.lock()->get_private_api().on_dock_area_closing(std::static_pointer_cast<SDockingArea>(shared_from_this()));
		
					parent_window->request_destroy_window();
				}
				else if (removal_method == TabRemoval_DraggedOut)
				{
					//we can't actually destroy this due to limitations of some platforms
					//just hide the window, we will destroy when the drag and drop is done
					//parent_window->request_destroy_window();
					m_b_clenup_upon_tab_relocation = true;

					m_my_tab_manager.lock()->get_private_api().on_dock_area_closing(std::static_pointer_cast<SDockingArea>(shared_from_this()));
				}
			}
		}
		else
		{
			m_b_is_center_target_visible = false;
		}
		//in some cases a dock area will control the window
		//and we need to move some of the tabs out of the way to make room for window chrome
		update_window_chrome_and_side_bar();
	}
	void SDockingArea::update_window_chrome_and_side_bar()
	{
		std::vector<std::shared_ptr<SDockingNode>> all_nodes = this->get_child_nodes_recursively();

		if (all_nodes.size() > 0)
		{
			//clean out all the reserved space
			for (auto some_node : all_nodes)
			{
				if (some_node->get_node_type() == DockTabStack)
				{
					auto some_tab_stack = std::static_pointer_cast<SDockingTabStack>(some_node);
					some_tab_stack->clear_reserved_space();
				}
			}

			//todo:add b can have side bar

			if (std::shared_ptr<SWindow> parent_window = get_parent_window())
			{
				//reserve some space for the minimize, restore, and close controls
				std::shared_ptr<SDockingTabStack> window_control_housing = this->find_tab_stack_to_house_window_controls();
				window_control_housing->reserve_space_for_window_chrome(SDockingTabStack::EChromeElement::Controls, false, false);

				//reserve some space for the app icons
				std::shared_ptr<SDockingTabStack> icon_housing = this->find_tab_stack_to_house_window_icon();
				icon_housing->reserve_space_for_window_chrome(SDockingTabStack::EChromeElement::Icon, true, true);//todo:fix me
			}
		}
		
	}

	void SDockingArea::on_tab_found_new_home(const std::shared_ptr<SDockTab>& relocated_tab, const std::shared_ptr<SWindow>& new_owner_window)
	{
		//the last tab has been successfully relocated else where
		//destroy this window
		std::shared_ptr<SWindow> parent_window = m_parent_window_ptr.lock();

		if (m_b_clenup_upon_tab_relocation && parent_window != new_owner_window)
		{
			//todo:add set request destroy window override
			parent_window->request_destroy_window();
		}
	}

	SDockingNode::ECleanupRetVal SDockingArea::clean_up_nodes()
	{
		SDockingNode::ECleanupRetVal return_value = SDockingSplitter::clean_up_nodes();

		return return_value;
	}

	EVisibility SDockingArea::target_cross_visibility() const
	{
		return (m_b_is_overlay_visible && !m_b_is_center_target_visible)
			? EVisibility::visible
			: EVisibility::Collapsed;
	}

	EVisibility SDockingArea::target_cross_center_visibility() const
	{
		return (m_b_is_overlay_visible && m_b_is_center_target_visible)
			? EVisibility::visible
			: EVisibility::Collapsed;
	}

}
