#include <PreCompileHeader.h>

#include "TabManager.h"

#include "SDockingArea.h"

#include "SDockingTabStack.h"

#include "SlateCore/Widgets/SWindow.h"//SWindow

#include "Application/Application.h"//create new os window

#include "Slate/Widgets/Docking/SDockTab.h"//SDockTab depends on it

namespace DoDo
{
	//------as function------
	std::shared_ptr<FTabManager::FStack> FTabManager::FLayoutNode::as_stack()
	{
		return std::shared_ptr<FTabManager::FStack>();//it is nullptr
	}

	std::shared_ptr<FTabManager::FSplitter> FTabManager::FLayoutNode::as_splitter()
	{
		return std::shared_ptr<FTabManager::FSplitter>();
	}

	std::shared_ptr<FTabManager::FArea> FTabManager::FLayoutNode::as_area()
	{
		return std::shared_ptr<FTabManager::FArea>();
	}
	//------as function------

	bool FTabManager::FPrivateApi::can_tab_leave_tab_well(const std::shared_ptr<const SDockTab>& tab_to_test) const
	{
		return m_tab_manager.m_b_can_do_drag_operation;//todo:add check this tab is main non closeable tab
	}

	FTabManager::FPrivateApi& FTabManager::get_private_api()
	{
		return *m_private_api;
	}

	FTabManager::FTabManager(const std::shared_ptr<SDockTab>& in_owner_tab, const std::shared_ptr<FTabManager::FTabSpawner>& in_nomad_tab_spawner)
		: m_nomed_tab_spawner(in_nomad_tab_spawner)
		, m_private_api(std::make_shared<FPrivateApi>(*this))
		, m_b_can_do_drag_operation(true)//todo:add owner tab ptr
	{
	}

	std::shared_ptr<SDockingArea> FTabManager::restore_area(const std::shared_ptr<FArea>& area_to_restore, const std::shared_ptr<SWindow>& in_parent_window, const bool b_embed_title_area_content, const EOutputCanBeNullptr out_put_can_be_nullptr)
	{
		//sidebar tabs for this area
		FSidebarTabLists side_bar_tabs;//this is tab bar vector

		m_temporarily_sidebared_tabs.clear();

		if (std::shared_ptr<SDockingNode> restore_node = restore_area_helper(area_to_restore, in_parent_window, b_embed_title_area_content, side_bar_tabs, out_put_can_be_nullptr))
		{
			std::shared_ptr<SDockingArea> restored_area = std::static_pointer_cast<SDockingArea>(restore_node);

			//todo:implement clean up
			restored_area->clean_up(SDockingNode::TabRemoval_None);//note:will add some padding from app icon

			restored_area->add_side_bar_tabs_from_restored_layout(side_bar_tabs);

			return restored_area;
		}
		else
		{
			//todo:check
			return nullptr;
		}
	}

	std::shared_ptr<SDockingNode> FTabManager::restore_area_helper(const std::shared_ptr<FLayoutNode>& layout_node, const std::shared_ptr<SWindow>& parent_window, const bool b_embed_title_area_content, FSidebarTabLists& out_sidebar_tabs, const EOutputCanBeNullptr output_can_be_nullptr)
	{
		std::shared_ptr<FTabManager::FStack> node_as_stack = layout_node->as_stack();
		std::shared_ptr<FTabManager::FSplitter> node_as_splitter = layout_node->as_splitter();
		std::shared_ptr<FTabManager::FArea> node_as_area = layout_node->as_area();

		const bool b_can_out_put_be_nullptr = (output_can_be_nullptr != EOutputCanBeNullptr::Never);

		if (node_as_stack)
		{
			std::shared_ptr<SDockTab> widget_to_activate;

			std::shared_ptr<SDockingTabStack> new_stack_widget;//this is a collection of SDockTab

			//should we init new stack widget before the for loop?
			//it depends on OutputCanBeNullptr
			bool b_is_new_stack_widget_init = false;
			//1. if EOutputCanBeNullptr::Never, function cannot return nullptr
			if (output_can_be_nullptr == EOutputCanBeNullptr::Never)
			{
				b_is_new_stack_widget_init = true;
			}
			//2.if EOuputCanBeNullptr::IfNoTabValid, we must init the SWidget as soon as any tab is valid for spawning
			else if (output_can_be_nullptr == EOutputCanBeNullptr::IfNoTabValid)
			{
				//note: is valid tab for spawning does not check whether spawn tab() will return nullptr
				for (const FTab& some_tab : node_as_stack->m_tabs)
				{
					if (is_valid_tab_for_spawning(some_tab))
					{
						b_is_new_stack_widget_init = true;
						break;
					}
				}
			}
			//3.if EOuputCanBeNullptr::IfNoOpenTabValid, we must init the SWidget as soon as any open tab is valid for spawning, for efficiency, done in the for loop
			//4.else, case not handled->error
			else if (output_can_be_nullptr != EOutputCanBeNullptr::IfNoOpenTabValid)
			{
				//todo:check
			}
			
			//initialize the SWidget already?
			if (b_is_new_stack_widget_init)
			{
				new_stack_widget = SNew(SDockingTabStack, node_as_stack);
				new_stack_widget->set_size_coefficient(layout_node->get_size_coefficient());
			}

			//open tabs
			for (const FTab& some_tab : node_as_stack->m_tabs)
			{
				if ((some_tab.m_tab_state == ETabState::OpenedTab || some_tab.m_tab_state == ETabState::SidebarTab) && is_valid_tab_for_spawning(some_tab))
				{
					const std::shared_ptr<SDockTab> new_tab_widget = spawn_tab(some_tab.m_tab_id, parent_window, b_can_out_put_be_nullptr);//todo:implement SpanwTab

					if (new_tab_widget)
					{
						if (some_tab.m_tab_id == node_as_stack->m_foreground_tab_id)
						{
							widget_to_activate = new_tab_widget;//most front tab, active
						}

						//first time initialization, only if at least a valid new tab widget
						if (!new_stack_widget)
						{
							new_stack_widget = SNew(SDockingTabStack, node_as_stack);
							new_stack_widget->set_size_coefficient(layout_node->get_size_coefficient());
						}

						if (some_tab.m_tab_state == ETabState::OpenedTab)
						{
							//todo:implement add tab widget
							new_stack_widget->add_tab_widget(new_tab_widget);
						}
						else
						{
							//let the stack know we have a tab that belongs in its stack that is currently in a sidebar
							//todo:implement add sidebar
							if (some_tab.m_side_bar_location == ESidebarLocation::left)
							{
								out_sidebar_tabs.m_left_side_bar_tabs.push_back(new_tab_widget);
							}
							else
							{
								out_sidebar_tabs.m_right_side_bar_tabs.push_back(new_tab_widget);
							}
						}
					}
				}
			}

			if (widget_to_activate)
			{
				//todo:implement active in parent


			}

			return new_stack_widget;
		}
		else if (node_as_area)
		{
			const bool b_splitter_is_dock_area = node_as_area != nullptr;
			const bool b_dock_needs_new_window = (node_as_area != nullptr) && (node_as_area->m_window_placement != FArea::Placement_NoWindow);

			std::shared_ptr<SDockingArea> new_dock_area_widget;

			if (b_dock_needs_new_window)
			{
				//the layout node we are restoring is a dock area
				//it need a new window into which it will land
				const bool b_is_child_window = parent_window != nullptr;

				const bool b_auto_placement = (node_as_area->m_window_placement == FArea::Placement_Automatic);

				std::shared_ptr<SWindow> new_window = b_auto_placement
					? SNew(SWindow)
					.ClientSize(node_as_area->m_unscaled_window_size)
					.CreateTitleBar(false)//note:this title bar is created from SDockArea
					: SNew(SWindow)
					.ScreenPosition(node_as_area->m_unscaled_window_position)
					.ClientSize(node_as_area->m_unscaled_window_size)
					.CreateTitleBar(false);

				//set a default title, restoring the splitter content may override this if it actives a tab
				//todo:set title
				new_window->set_title(FGlobalTabmanager::get()->get_application_title());

				std::vector<std::shared_ptr<SDockingNode>> docking_nodes;
				//todo:implement can restore splitter content
				if (can_restore_splitter_content(docking_nodes, node_as_area, new_window, out_sidebar_tabs, output_can_be_nullptr))
				{
					//create splitter widget only if it will be filled with at least 1 docking nodes
					//any windows that were "pulled out" of a dock area should be children of the window in which the parent dock area resides
					if (b_is_child_window)
					{
						//todo:implement add window as native child
					}
					else
					{
						Application::get().add_window(new_window)->set_content(
							SAssignNew(new_dock_area_widget, SDockingArea, shared_from_this(), node_as_area).ParentWindow(new_window)
						);
					}

					//restore content
					if (!b_can_out_put_be_nullptr)
					{
						restore_splitter_content(node_as_area, new_dock_area_widget, new_window, out_sidebar_tabs);
					}
					else
					{
						restore_splitter_content(docking_nodes, new_dock_area_widget);
					}
				}
			}
			else
			{
				std::vector<std::shared_ptr<SDockingNode>> docking_nodes;//don't create new window
				
				//can restore splitter content will recursive to call restore_area_helper function
				if (can_restore_splitter_content(docking_nodes, node_as_area, parent_window, out_sidebar_tabs, output_can_be_nullptr))
				{
					SAssignNew(new_dock_area_widget, SDockingArea, shared_from_this(), node_as_area)
						//we only want to set a parent window on this dock area, if we need to have title area content
						//embedded within it, SDockingArea assumes that if it has a parent window set, then it needs to have
						//title area content
						.ParentWindow(b_embed_title_area_content ? parent_window : std::shared_ptr<SWindow>())
						//never manage these windows, even if a parent window is set, the owner will take care of
						//destroying these windows
						.ShouldManageParentWindow(false);

					//restore content
					if (!b_can_out_put_be_nullptr)
					{
						restore_splitter_content(node_as_area, new_dock_area_widget, parent_window, out_sidebar_tabs);
					}
					else
					{
						restore_splitter_content(docking_nodes, new_dock_area_widget);
					}
				}
			}
			return new_dock_area_widget;
		}
		else if (node_as_splitter)
		{
			std::vector<std::shared_ptr<SDockingNode>> docking_nodes;
			//todo:implement can restore splitter content
			if (can_restore_splitter_content(docking_nodes, node_as_splitter, parent_window, out_sidebar_tabs, output_can_be_nullptr))
			{
				std::shared_ptr<SDockingSplitter> new_splitter_widget = SNew(SDockingSplitter, node_as_splitter);

				new_splitter_widget->set_size_coefficient(layout_node->get_size_coefficient());

				//restore content
				if (!b_can_out_put_be_nullptr)
				{
					restore_splitter_content(node_as_splitter, new_splitter_widget, parent_window, out_sidebar_tabs);
				}
				else
				{
					restore_splitter_content(docking_nodes, new_splitter_widget);
				}
				return new_splitter_widget;
			}
			else
			{
				return nullptr;
			}
		}
		else
		{
			//todo:implement this

		}
	}

	std::shared_ptr<SDockTab> FTabManager::spawn_tab(const FTabId& tab_id, const std::shared_ptr<SWindow>& parent_window, const bool b_can_output_be_nullptr)
	{
		std::shared_ptr<SDockTab> new_tab_widget;

		//whether or not the spawner override the ability for the tab to even spawn, this is not a failure case
		bool b_spawning_allowed_by_spawner = true;

		//do we know how to spawn such a tab?
		std::shared_ptr<FTabSpawnerEntry> spawner = find_tab_spawner_for(tab_id.m_tab_type);

		if (spawner)
		{
			if (spawner->m_can_spawn_tab.is_bound())
			{
				b_spawning_allowed_by_spawner = spawner->m_can_spawn_tab.execute(FSpawnTabArgs(parent_window, tab_id));
			}

			//todo:add more logic
			if (b_spawning_allowed_by_spawner && (spawner->m_spawned_tab_ptr.lock() == nullptr))//todo:add on find tab to reuse
			{
				new_tab_widget = spawner->m_on_spawn_tab.execute(FSpawnTabArgs(parent_window, tab_id));

				//todo:set layout identifier and provide default local and icon

				new_tab_widget->provide_default_icon(spawner->get_icon().get_icon());//note:this is important

				new_tab_widget->provide_default_label(spawner->get_display_name());

				//the spawner tracks that last tab it spawned
				spawner->m_spawned_tab_ptr = new_tab_widget;
			}
			else
			{
				//if we got here, somehow there is two entries spawning the same tab, this is now allowed so just ignore it
				b_spawning_allowed_by_spawner = false;
			}
		}

		//todo:add more logic
		if (new_tab_widget)
		{
			new_tab_widget->set_tab_manager(shared_from_this());
		}

		return new_tab_widget;
	}

	bool FTabManager::has_valid_open_tabs(const std::shared_ptr<FTabManager::FLayoutNode>& some_node) const
	{
		//search for valid and open tabs, search state is open tab
		struct OpenTabMatcher
		{
			const FTabManager* m_tab_manager;

			bool operator()(const FTab& candidate) const
			{
				return m_tab_manager->is_valid_tab_for_spawning(candidate) && candidate.m_tab_state == ETabState::OpenedTab;
			}
		};

		OpenTabMatcher find_open_tab;
		find_open_tab.m_tab_manager = this;

		return has_any_matching_tabs(some_node, find_open_tab);
	}

	std::shared_ptr<SWidget> FTabManager::restore_from(const std::shared_ptr<FLayout>& layout,
		const std::shared_ptr<SWindow>& parent_window, const bool b_embed_title_area_content,
		const EOutputCanBeNullptr restore_are_out_put_can_be_nullptr)
	{
		m_active_layout_name = layout->m_layout_name;//store the FLayout layout name

		std::shared_ptr<SDockingArea> primary_dock_area;//SDockingArea is inherited from SDockingSplitter

		for(int32_t area_index = 0; area_index < layout->m_areas.size(); ++area_index) //note:from layout get FArea to traverse
		{
			const std::shared_ptr<FArea>& this_area = layout->m_areas[area_index];//get current FArea

			//set all invalid tab tabs to opened tab so the editor tries to load them, all non-recognized tabs will set to invalid tab later
			//note:change tab state
			set_tabs_on(this_area, ETabState::OpenedTab, ETabState::InvalidTab);//note:recursive to modify tab state

			const bool b_is_primary_area = this_area->m_window_placement == FArea::Placement_NoWindow;
			const bool b_should_create = b_is_primary_area || has_valid_tabs(this_area);//todo:implement has valid tabs
			//note:check this FArea have tab spawner entry

			if(b_should_create)
			{
				std::shared_ptr<SDockingArea> restored_area;
				const bool b_has_valid_open_tabs = b_is_primary_area || has_valid_open_tabs(this_area);//todo:implement has valid open tabs

				if(b_has_valid_open_tabs)
				{
					//todo:implement restore area
					restored_area = restore_area(this_area, parent_window, b_embed_title_area_content, restore_are_out_put_can_be_nullptr);
					//invalidate all tabs in this area because they were not recognized
					if (!restored_area)
					{
						set_tabs_on(this_area, ETabState::InvalidTab, ETabState::OpenedTab);
						//todo:push this area to invalid dock areas
					}
				}
				else
				{
					
				}

				if (b_is_primary_area && restored_area && !primary_dock_area)
				{
					primary_dock_area = restored_area;//set restore are to primary
				}
			}
		}

		//todo:implement update stats

		//todo:implement finish restore

		return primary_dock_area;
	}

	std::shared_ptr<FTabSpawnerEntry> FTabManager::find_tab_spawner_for(const DoDoUtf8String& tab_id)
	{
		//look for a spawner in this tab manager
		auto it = m_tab_spawner.find(tab_id);

		if (it != m_tab_spawner.end())
		{
			return it->second;
		}
		else
		{
			//to find nomad tab spawner
			auto it2 = m_nomed_tab_spawner->find(tab_id);
			
			return it2->second;
		}

		return std::shared_ptr<FTabSpawnerEntry>();
	}

	bool FTabManager::can_restore_splitter_content(std::vector<std::shared_ptr<SDockingNode>>& docking_nodes, const std::shared_ptr<FSplitter>& splitter_node, const std::shared_ptr<SWindow>& parent_window, FSidebarTabLists& out_side_bar_tabs, const EOutputCanBeNullptr out_put_can_be_nullptr)
	{
		if (out_put_can_be_nullptr == EOutputCanBeNullptr::Never)
		{
			return true;
		}
		docking_nodes.clear();

		//restore the contents of this splitter
		for (int32_t child_node_index = 0; child_node_index < splitter_node->m_child_nodes.size(); ++child_node_index)
		{
			const std::shared_ptr<FLayoutNode> this_child_node = splitter_node->m_child_nodes[child_node_index];

			const bool b_embed_title_area_content = false;

			//recursive
			const std::shared_ptr<SDockingNode> this_child_node_widget = restore_area_helper(this_child_node, parent_window, 
			b_embed_title_area_content, out_side_bar_tabs, out_put_can_be_nullptr);

			if (this_child_node_widget)
			{
				const std::shared_ptr<SDockingNode> this_child_node = std::static_pointer_cast<SDockingNode>(this_child_node_widget);
				docking_nodes.push_back(this_child_node_widget);
			}
		}

		return docking_nodes.size() > 0;
	}

	void FTabManager::restore_splitter_content(const std::vector<std::shared_ptr<SDockingNode>>& docking_nodes, const std::shared_ptr<SDockingSplitter>& splitter_widget)
	{
		for (const std::shared_ptr<SDockingNode>& docking_node : docking_nodes)
		{
			//todo:implement add child node
		}
	}

	void FTabManager::restore_splitter_content(const std::shared_ptr<FSplitter>& splitter_node, const std::shared_ptr<SDockingSplitter>& splitter_widget, const std::shared_ptr<SWindow>& parent_window, FSidebarTabLists& out_sidebar_tabs)
	{
		//restore the contents of this splitter
		for (int32_t child_node_index = 0; child_node_index < splitter_node->m_child_nodes.size(); ++child_node_index)
		{
			std::shared_ptr<FLayoutNode> this_child_node = splitter_node->m_child_nodes[child_node_index];

			const bool b_embed_title_area_content = false;
			std::shared_ptr<SDockingNode> this_child_node_widget = restore_area_helper(this_child_node, parent_window, b_embed_title_area_content, out_sidebar_tabs);

			if (this_child_node_widget != nullptr)
			{
				const std::shared_ptr<SDockingNode> child_node_widget = std::static_pointer_cast<SDockingNode>(this_child_node_widget);
				//todo:implement add child node
				splitter_widget->add_child_node(child_node_widget, -1);
			}
		}
	}

	bool FTabManager::is_valid_tab_for_spawning(const FTab& some_tab) const
	{
		if (!is_allowed_tab(some_tab.m_tab_id))
		{
			return false;
		}

		//nomed tabs beings restored from layouts should not be spawned if the nomed tab is already spawned
		//std::shared_ptr<FTabSpawnerEntry>* nomed_spanwer =

		std::shared_ptr<FTabSpawnerEntry>* nomed_spawner = nullptr;

		auto it = m_nomed_tab_spawner->find(some_tab.m_tab_id.m_tab_type);
		if (it != m_nomed_tab_spawner->end())
		{
			nomed_spawner = &(it->second);
		}

		return (!nomed_spawner || !(nomed_spawner->get()->is_sole_tab_instance_spawned()));//todo:implement is sole tab instance spawned
	}

	bool FTabManager::is_allowed_tab(const FTabId& tab_id) const
	{
		return is_allowed_tab_type(tab_id.m_tab_type);
	}

	bool FTabManager::is_allowed_tab_type(const DoDoUtf8String tab_type) const
	{
		//todo:check tab type
		const bool b_is_allowed = true;//todo:implement tab permission list
		if (!b_is_allowed)
		{
			//todo:log warn
		}
		return b_is_allowed;
	}

	template<typename MatchFunctorType>
	bool FTabManager::has_any_matching_tabs(const std::shared_ptr<FTabManager::FLayoutNode>& some_node, const MatchFunctorType& matcher)
	{
		std::shared_ptr<FTabManager::FSplitter> as_splitter = some_node->as_splitter();
		std::shared_ptr<FTabManager::FStack> as_stack = some_node->as_stack();//note:as just return empty

		if(as_stack)
		{
			//todo:index of by predicate
			return true;
		}
		else
		{
			//do any of the child nodes have open tabs?
			for(int32_t child_index = 0; child_index < as_splitter->m_child_nodes.size(); ++child_index)
			{
				if(has_any_matching_tabs(as_splitter->m_child_nodes[child_index], matcher))//recursive
				{
					return true;
				}
			}
			return false;
		}
	}

	bool FTabManager::has_valid_tabs(const std::shared_ptr<FTabManager::FLayoutNode>& some_node) const
	{
		//note:search for valid tabs that can be spawned
		struct ValidTabMatcher
		{
			const FTabManager* m_tab_manager;

			bool operator()(const FTab& candidate) const
			{
				//todo:implement is valid tab for spawning
				return m_tab_manager->is_valid_tab_for_spawning(candidate);//note:check tab's type have tab spawner entry
			}
		};
		ValidTabMatcher find_valid_tab;
		find_valid_tab.m_tab_manager = this;


		return has_any_matching_tabs(some_node, find_valid_tab);
	}

	void FTabManager::set_tabs_on(const std::shared_ptr<FTabManager::FLayoutNode>& some_node,
	                              const ETabState::Type new_tab_state, const ETabState::Type original_tab_state) const
	{
		//set particular tab to desired new tab state
		std::shared_ptr<FTabManager::FStack> as_stack = some_node->as_stack();

		//note:if some node is FStack, FStack is a collection of FTab, to change their states

		if(as_stack)
		{
			std::vector<FTab>& tabs = as_stack->m_tabs;//note:get FStack's FTabs
			for(int32_t tab_index = 0; tab_index < tabs.size(); ++tab_index)
			{
				if(tabs[tab_index].m_tab_state == original_tab_state)
				{
					tabs[tab_index].m_tab_state = new_tab_state;//note:change state
				}
			}
		}//note:recursively set all tabs to desired new tab state
		else
		{
			std::shared_ptr<FTabManager::FSplitter> as_splitter = some_node->as_splitter();//covert to the FSplitter

			for(int32_t child_index = 0; child_index < as_splitter->m_child_nodes.size(); ++child_index)
			{
				set_tabs_on(as_splitter->m_child_nodes[child_index], new_tab_state, original_tab_state);//change state
			}
		}
	}
	const std::shared_ptr<FGlobalTabmanager>& FGlobalTabmanager::get()
	{
		static const std::shared_ptr<FGlobalTabmanager> instance = FGlobalTabmanager::New();

		return instance;
	}
	FTabSpawnerEntry& FGlobalTabmanager::register_nomad_tab_spawner(const DoDoUtf8String& tab_id, const FOnSpawnTab& on_spawn_tab, const FCanSpawnTab& can_spawn_tab)
	{
		//todo:remove tab id if it was previously loaded, this allows re-loading the Editor UI layout without restarting the whole Editor

		//re(create) and return NewSpawnerEntry
		std::shared_ptr<FTabSpawnerEntry> new_spawner_entry = std::make_shared<FTabSpawnerEntry>(tab_id, on_spawn_tab, can_spawn_tab);

		m_nomed_tab_spawner->insert({ tab_id, new_spawner_entry });

		return *new_spawner_entry;
	}
	const DoDoUtf8String& FGlobalTabmanager::get_application_title() const
	{
		return m_app_title;
	}
	void FGlobalTabmanager::set_application_title(const DoDoUtf8String& app_title)
	{
		m_app_title = app_title;

		//todo:check
		//for(int32_t dock_area_index = 0; dock_area_index < 
	}
	FTabSpawnerEntry& FTabSpawnerEntry::set_icon(const FSlateIcon& in_icon)
	{
		m_icon = in_icon;
		return *this;
	}

	FTabSpawnerEntry& FTabSpawnerEntry::set_display_name(const DoDoUtf8String& in_legible_name)
	{
		m_display_name_attribute = in_legible_name;
		return *this;
	}
}
