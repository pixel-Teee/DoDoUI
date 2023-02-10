#include <PreCompileHeader.h>

#include "TabManager.h"

#include "SDockingArea.h"

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
			set_tabs_on(this_area, ETabState::OpenedTab, ETabState::InvalidTab);

			const bool b_is_primary_area = this_area->m_window_placement == FArea::Placement_NoWindow;
			const bool b_should_create = b_is_primary_area;//todo:implement has valid tabs

			if(b_should_create)
			{
				std::shared_ptr<SDockingArea> restored_area;
				const bool b_has_valid_open_tabs = b_is_primary_area;//todo:implement has valid open tabs

				if(b_has_valid_open_tabs)
				{
					//todo:implement restore area
				}
				else
				{
					
				}
			}
		}

		//todo:implement update stats

		//todo:implement finish restore

		return primary_dock_area;
	}

	template<typename MatchFunctorType>
	bool FTabManager::has_any_matching_tabs(const std::shared_ptr<FTabManager::FLayoutNode>& some_node, const MatchFunctorType& matcher)
	{
		std::shared_ptr<FTabManager::FSplitter> as_splitter = some_node->as_splitter();
		std::shared_ptr<FTabManager::FStack> as_stack = some_node->as_stack();//note:as just return empty

		if(as_stack)
		{
			//todo:index of by predicate
		}
		else
		{
			//do any of the child nodes have open tabs?
			for(int32_t child_index = 0; child_index < as_splitter->m_child_nodes.size(); ++child_index)
			{
				if(has_any_matching_types(as_splitter->m_child_nodes[child_index], matcher))//recursive
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
				return true;
			}
		};
		ValidTabMatcher find_valid_tab;
		find_valid_tab.m_tab_manager = this;


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
}
