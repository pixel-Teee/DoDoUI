#pragma once

#include <memory>

#include "Core/String/DoDoString.h"

#include "SlateCore/Types/SlateEnums.h"//EOrientation depends on it

#include "glm/glm.hpp"

#include "WorkspaceItem.h"//FTabSpawnerEntry depends on it

//#include "Slate/Widgets/Docking/SDockTab.h"

//#include "Slate/Framework/Docking/SDockingTabStack.h"//FTabMatcher

#include "Core/Delegates/DelegateCombinations.h"

namespace DoDo
{
	class SWindow;
	class SWidget;
	class SDockingArea;
	class SDockingNode;
	class SDockingSplitter;
	class SDockTab;
	struct FTabMatcher;
	struct FSidebarTabLists;

	enum class ESidebarLocation : uint8_t
	{
		/*tab is in a sidebar on the left side of its parent area*/
		left,

		/*tab is in a sidebar on the right side of its parent area*/
		right,

		/*tab is not in a sidebar*/
		none,
	};

	namespace ETabState
	{
		enum Type
		{
			OpenedTab = 0x1 << 0,
			ClosedTab = 0x1 << 1,
			SidebarTab = 0x1 << 2,

			/*
			 * invalid tab refers to tabs that were not recognized by the editor (e.g., live link when it's plugin its disabled)
			 */
			InvalidTab = 0x1 << 3
		};
	}

	enum class ETabIdFlags : uint8_t
	{
		None = 0x0, //no flags
		SaveLayout = 0x1,//this tab should be included when saving the slate layout
	};

	struct FTabId
	{
		FTabId(const DoDoUtf8String& in_tab_type)
			: m_tab_type(in_tab_type)
			, m_instance_id(-1)
			, m_flags(ETabIdFlags::SaveLayout)
		{}

		bool operator==(const FTabId& other) const
		{
			return m_tab_type == other.m_tab_type && (m_instance_id == -1 || other.m_instance_id == -1 || m_instance_id == other.m_instance_id);
		}
		DoDoUtf8String m_tab_type;
		int32_t m_instance_id;

	private:
		ETabIdFlags m_flags;//note:need to save the tab?
	};

	class FSpawnTabArgs
	{
	public:
		FSpawnTabArgs(const std::shared_ptr<SWindow>& in_owner_window, const FTabId& in_tab_being_spawned_id)
			: m_tab_id_being_spawned(in_tab_being_spawned_id)
			, m_owner_window(in_owner_window)
		{}

		const std::shared_ptr<SWindow>& get_owner_window() const
		{
			return m_owner_window;
		}

		const FTabId& get_tab_id() const
		{
			return m_tab_id_being_spawned;
		}
	private:
		FTabId m_tab_id_being_spawned;
		std::shared_ptr<SWindow> m_owner_window;
	};

	/*
	* invoked when a tab needs to be spawned
	*/
	DECLARE_DELEGATE_RetVal_OneParam(std::shared_ptr<SDockTab>, FOnSpawnTab, const FSpawnTabArgs&);
	DECLARE_DELEGATE_RetVal_OneParam(bool, FCanSpawnTab, const FSpawnTabArgs&);

	struct FTabSpawnerEntry : public FWorkspaceItem
	{
		FTabSpawnerEntry(const DoDoUtf8String& in_tab_type, const FOnSpawnTab& in_spawn_tab_method, const FCanSpawnTab& in_can_spawn_tab)
			: FWorkspaceItem(DoDoUtf8String(), FSlateIcon(), false)
			, m_tab_type(in_tab_type)
			, m_on_spawn_tab(in_spawn_tab_method)
			, m_can_spawn_tab(in_can_spawn_tab)
			, m_spawned_tab_ptr()
		{}

		FTabSpawnerEntry& set_icon(const FSlateIcon& in_icon);

		FTabSpawnerEntry& set_display_name(const DoDoUtf8String& in_legible_name);
	private:
		DoDoUtf8String m_tab_type;//tab type
		FOnSpawnTab m_on_spawn_tab;
		FCanSpawnTab m_can_spawn_tab;

		std::weak_ptr<SDockTab> m_spawned_tab_ptr;//todo:implement this

		bool is_sole_tab_instance_spawned() const
		{
			//items that allow multiple instances need a custom way to find and reuse tabs
			return m_spawned_tab_ptr.lock() != nullptr;
		}

		friend class FTabManager;
	};

	enum class EOutputCanBeNullptr//note:restore_from function return value is nullptr?
	{
		/* RestoreArea_Helper() will always return a SWidget, it will return an "unrecognized tab" dummy SWidget if it cannot find the way to create the desired SWidget
		 * default behavior and the only one used before UE 4.24
		 * this is the most strict condition and will never return nullptr
		 */
		Never,
		/* RestoreArea_Helper() will return nullptr if its parent FTabManager::FStack does not contain at least a valid tab
		 * useful for docked SWidgets that contain closed tabs
		 * with IfNoTabValid, if a previously docked Tab is re-opened (i.e. its ETabState value changes from ClosedTab to OpenedTab), it will be displayed in the same place
		 * it was placed before being initially closed, however, with IfNonOpenTabValid, if that Tab were re-opened, it might be instead displayed in a new standalone SWidget
		 */
		IfNoTabValid,
		/* RestoreArea_Helper() will return nullptr if its parent FTabManager::FStack does not contain at least a valid tab whose ETabState is set to OpenedTab
		 * useful for standalone SWidgets that otherwise will display a blank UI with no tabs on it
		 * this is the most relaxed condition and the one that will return nullptr the most
		 */
		IfNoOpenTabValid
	};

	class FTabManager : public std::enable_shared_from_this<FTabManager>
	{
	public:

		//------forward declare------
		class FStack;
		class FSplitter;
		class FArea;
		class FLayout;
		//------forward declare------

		//------the base class of FStack FSplitter FArea FLayout------
		class FLayoutNode : public std::enable_shared_from_this<FLayoutNode>
		{
			friend class FTabManager;
		public:
			virtual ~FLayoutNode() {}

			virtual std::shared_ptr<FStack> as_stack();//as stack

			virtual std::shared_ptr<FSplitter> as_splitter();//as splitter

			virtual std::shared_ptr<FArea> as_area();//as area

			float get_size_coefficient() const { return m_size_coefficient; }

		protected:
			FLayoutNode()
				: m_size_coefficient(1.0f)
			{}

			float m_size_coefficient;
		};
		//------the base class of FStack FSplitter FArea FLayout------

		//todo:implement FTab and FStack
		struct FTab
		{
			FTab(const FTabId& in_tab_id, ETabState::Type in_tab_state)
				: m_tab_id(in_tab_id)
				, m_tab_state(in_tab_state)
				, m_side_bar_location(ESidebarLocation::none)
				, m_side_bar_size_coefficient(0.0f)
				, m_b_pinned_in_side_bar(false)
			{}

			FTabId m_tab_id;//tab id, need save?
			ETabState::Type m_tab_state;//the tab state, open or close or sidebar?
			ESidebarLocation m_side_bar_location;//tab sidebar location
			float m_side_bar_size_coefficient;//bar size coefficient
			bool m_b_pinned_in_side_bar;//todo:how to understand it?
		};

		class FStack : public FLayoutNode
		{
			friend class FTabManager;
			friend class FLayout;
			friend class SDockingTabStack;
		public:
			std::shared_ptr<FStack> add_tab(const DoDoUtf8String& tab_type, ETabState::Type in_tab_state)
			{
				m_tabs.push_back(FTab(FTabId(tab_type), in_tab_state));

				return std::static_pointer_cast<FStack>(shared_from_this());
			}

			std::shared_ptr<FStack> add_tab(const FTab& tab)
			{
				m_tabs.push_back(tab);
				return std::static_pointer_cast<FStack>(shared_from_this());
			}

			std::shared_ptr<FStack> set_foreground_tab(const FTabId& tab_id)
			{
				m_foreground_tab_id = tab_id;
				return std::static_pointer_cast<FStack>(shared_from_this());
			}

			virtual std::shared_ptr<FStack> as_stack() override
			{
				return std::static_pointer_cast<FStack>(shared_from_this());
			}

			std::shared_ptr<FStack> set_size_coefficient(const float in_size_coefficient)
			{
				m_size_coefficient = in_size_coefficient;
				return std::static_pointer_cast<FStack>(shared_from_this());
			}

			virtual ~FStack()
			{

			}
		
			FStack()
				: m_tabs()
				, m_b_hide_tab_well(false)
				, m_foreground_tab_id("")
			{}

		protected:
			std::vector<FTab> m_tabs;//a collection of FTab
			bool m_b_hide_tab_well;//todo:how to understand it
			FTabId m_foreground_tab_id;//foreground tab
			DoDoUtf8String m_extension_id;//extension id
		};

		class FSplitter : public FLayoutNode
		{
			friend class FTabManager;
		public:

			virtual ~FSplitter()
			{
				
			}

			FSplitter()
			: m_orientation(Orient_Horizontal)
			{
				
			}

			std::shared_ptr<FSplitter> split(std::shared_ptr<FLayoutNode> in_node)
			{
				m_child_nodes.push_back(in_node);
				return std::static_pointer_cast<FSplitter>(shared_from_this());
			}

			virtual std::shared_ptr<FSplitter> as_splitter() override
			{
				return std::static_pointer_cast<FSplitter>(shared_from_this());
			}

			std::shared_ptr<FSplitter> set_orientation(const EOrientation in_orientation)
			{
				m_orientation = in_orientation;
				return std::static_pointer_cast<FSplitter>(shared_from_this());
			}

			std::shared_ptr<FSplitter> set_size_coefficient(const float in_size_coefficient)
			{
				m_size_coefficient = in_size_coefficient;
				return std::static_pointer_cast<FSplitter>(shared_from_this());
			}

			EOrientation get_orientation() const { return m_orientation; }
		protected:
			EOrientation m_orientation;//have orientation
			std::vector<std::shared_ptr<FLayoutNode>> m_child_nodes;
		};

		class FArea : public FSplitter//FArea is inherited from FSplitter
		{
			friend class FTabManager;
			friend class FLayout;
		public:
			enum EWindowPlacement//todo:how to understand it?
			{
				Placement_NoWindow,
				Placement_Automatic,
				Placement_Specified
			};

			std::shared_ptr<FArea> split(std::shared_ptr<FLayoutNode> in_node)
			{
				m_child_nodes.push_back(in_node);
				return std::static_pointer_cast<FArea>(shared_from_this());
			}

			virtual std::shared_ptr<FArea> as_area() override
			{
				return std::static_pointer_cast<FArea>(shared_from_this());
			}

			std::shared_ptr<FArea> set_window(glm::vec2 in_position, bool is_maximized)
			{
				m_window_placement = Placement_Specified;
				m_unscaled_window_position = in_position;
				m_b_is_maximized = is_maximized;

				return std::static_pointer_cast<FArea>(shared_from_this());
			}

			virtual ~FArea()
			{

			}

			FArea(const float in_width, const float in_height)
				: m_window_placement(Placement_Automatic)
				, m_unscaled_window_position(glm::vec2(0.0f, 0.0f))
				, m_unscaled_window_size(glm::vec2(in_width, in_height))
				, m_b_is_maximized(false)
			{}

		protected:
			EWindowPlacement m_window_placement;//todo:how to understand it?
			glm::vec2 m_unscaled_window_position;
			glm::vec2 m_unscaled_window_size;
			bool m_b_is_maximized;
			DoDoUtf8String m_extension_id;
		};

		class FLayout : public std::enable_shared_from_this<FLayout>//note:this store the layout of FLayoutNode and it's derived class
		{
			friend class FTabManager;//grant the access power to the FTabManager
		public:
			std::shared_ptr<FLayout> add_area(const std::shared_ptr<FArea>& in_area)
			{
				m_areas.push_back(in_area);
				return std::static_pointer_cast<FLayout>(shared_from_this());
			}

			FLayout(const DoDoUtf8String& in_layout_name) : m_layout_name(in_layout_name)
			{}
		protected:
			std::weak_ptr<FArea> m_primary_area;
			std::vector<std::shared_ptr<FArea>> m_areas;
			/*the layout will be saved into a config file with this name, e.g. LevelEditorLayout or MaterialEditorLayout*/
			DoDoUtf8String m_layout_name;
		};

		friend class FPrivateApi;
		class FPrivateApi
		{
		public:
			FPrivateApi(FTabManager& in_tab_manager)
				: m_tab_manager(in_tab_manager)
			{}

			std::shared_ptr<SWindow> get_parent_window() const;

			void on_dock_area_created(const std::shared_ptr<SDockingArea>& newly_created_dock_area);

			/*notify the tab manager that a tab has been relocated, if the tab now lives in a new window, the new owner window should be a valid pointer*/
			bool can_tab_leave_tab_well(const std::shared_ptr<const SDockTab>& tab_to_test) const;
			
			void on_dock_area_closing(const std::shared_ptr<SDockingArea>& dock_area_that_is_closing);

			void on_tab_relocated(const std::shared_ptr<SDockTab>& relocated_tab, const std::shared_ptr<SWindow>& new_owner_window);
			/*notify the tab manager that a tab has been relocated, if the tab now lives in a new window, the new owner window should be a valid pointer*/
		private:
			FTabManager& m_tab_manager;
		};

		FTabManager::FPrivateApi& get_private_api();
	public:
		static std::shared_ptr<FLayout> new_layout(const DoDoUtf8String& layout_name)
		{
			return std::make_shared<FLayout>(layout_name);
		}

		static std::shared_ptr<FArea> new_primary_area()
		{
			std::shared_ptr<FArea> area = std::make_shared<FArea>(0, 0);
			area->m_window_placement = FArea::Placement_NoWindow;
			return area;
		}

		static std::shared_ptr<FArea> new_area(const float width, const float height)
		{
			return std::make_shared<FArea>(width, height);
		}

		static std::shared_ptr<FStack> new_stack()
		{
			return std::make_shared<FStack>();
		}

		static std::shared_ptr<FSplitter> new_splitter()
		{
			return std::make_shared<FSplitter>();
		}

		using FTabSpawner = std::map<DoDoUtf8String, std::shared_ptr<FTabSpawnerEntry>>;//key is tab id's tab type

		FTabManager(const std::shared_ptr<SDockTab>& in_owner_tab, const std::shared_ptr<FTabManager::FTabSpawner>& in_nomad_tab_spawner);

		/*return true if we can do drag operation*/
		bool get_can_do_drag_operation() { return m_b_can_do_drag_operation; }
	protected:
		
		std::shared_ptr<SDockingArea> restore_area(
			const std::shared_ptr<FArea>& area_to_restore, const std::shared_ptr<SWindow>& in_parent_window,
			const bool b_embed_title_area_content = false, const EOutputCanBeNullptr out_put_can_be_nullptr = EOutputCanBeNullptr::Never
		);//note:restore area, be called from restore_from


		std::shared_ptr<SDockingNode> restore_area_helper(const std::shared_ptr<FLayoutNode>& layout_node, const std::shared_ptr<SWindow>& parent_window,
			const bool b_embed_title_area_content, FSidebarTabLists& out_sidebar_tabs, const EOutputCanBeNullptr output_can_be_nullptr = EOutputCanBeNullptr::Never);

		std::shared_ptr<SDockTab> spawn_tab(const FTabId& tab_id, const std::shared_ptr<SWindow>& parent_window, const bool b_can_output_be_nullptr = false);
	public:
		/*
		* it searches for valid and open tabs on some node
		* @return it returns true if there is at least a valid open tab in the input some node
		*/
		bool has_valid_open_tabs(const std::shared_ptr<FTabManager::FLayoutNode>& some_node) const;

	public:
		std::shared_ptr<SWidget> restore_from(const std::shared_ptr<FLayout>& layout, const std::shared_ptr<SWindow>& parent_window,
		const bool b_embed_title_area_content = false, const EOutputCanBeNullptr restore_are_out_put_can_be_nullptr = EOutputCanBeNullptr::Never);//fourth parameter controls the return value
	protected:

		std::shared_ptr<FTabSpawnerEntry> find_tab_spawner_for(const DoDoUtf8String& tab_id);

		/*
		* use can restore splitter content + restore splitter content when the output of its internal restore area helper can be a nullptr
		* usage example:
		*	std::vector<std::shared_ptr<SDockingNode>> DockingNodes;
		*	if(CanRestoreSplitterContent(DockingNodes, SplitterNode, ParentWindow, OuputCanBeNullptr))
		*	{
		*		//create splitter widget only if it will be filled with at least 1 docking nodes
		*		std::shared_ptr<SDockingSplitter> SplitterWidget = SNew(SDockingSplitter, SplitterNode);
		*		//restore content
		*		RestoreSplitterContent(DockingNodes, SplitterWidget);
		*	}
		*/

		bool can_restore_splitter_content(std::vector<std::shared_ptr<SDockingNode>>& docking_nodes, const std::shared_ptr<FSplitter>& splitter_node,
			const std::shared_ptr<SWindow>& parent_window, FSidebarTabLists& out_side_bar_tabs, const EOutputCanBeNullptr out_put_can_be_nullptr);

		void restore_splitter_content(const std::vector<std::shared_ptr<SDockingNode>>& docking_nodes, const std::shared_ptr<SDockingSplitter>& splitter_widget);

		/*
		* use this standalone restore splitter content when the output of its internal restore area helper cannot be a nullptr
		*/
		void restore_splitter_content(const std::shared_ptr<FSplitter>& splitter_node, const std::shared_ptr<SDockingSplitter>& splitter_widget, const std::shared_ptr<SWindow>& parent_window,
			FSidebarTabLists& out_sidebar_tabs);

		bool is_valid_tab_for_spawning(const FTab& some_tab) const;
		bool is_allowed_tab(const FTabId& tab_id) const;
		bool is_allowed_tab_type(const DoDoUtf8String tab_type) const;//note:these are function to check tab is valid

		//note:pass predicate to check tab
		template<typename MatchFunctorType> static bool has_any_matching_tabs(const std::shared_ptr<FTabManager::FLayoutNode>& some_node, const MatchFunctorType& matcher);
	protected:
		bool has_valid_tabs(const std::shared_ptr<FTabManager::FLayoutNode>& some_node) const;

		/*
		 * it sets the desired (or all) tabs in the FTabManager::FLayoutNode to the desired value
		 * @param SomeNode the area whose tabs will be modified
		 * @param NewTabState the new tab state value
		 * @param OriginalTabState only the tabs with this value will be modified, use ETabState::AnyTab to modify them all
		 */
		//note:change the tab state
		void set_tabs_on(const std::shared_ptr<FTabManager::FLayoutNode>& some_node, const ETabState::Type new_tab_state, const ETabState::Type original_tab_state) const;

		/*
		* notify the tab manager that the new foreground tab was brought to front and the backgrounded tab was send to the background as a result
		*/
		virtual void on_tab_relocated(const std::shared_ptr<SDockTab>& relocated_tab, const std::shared_ptr<SWindow>& new_owner_window);

		static std::shared_ptr<FTabManager::FStack> find_tab_under_node(const FTabMatcher& matcher, const std::shared_ptr<FTabManager::FLayoutNode>& node_to_search_under);

		void remove_tab_from_collapsed_areas(const FTabMatcher& matcher);
	protected:
		FTabSpawner m_tab_spawner;//FTabSpawner is FTabSpawner entry map

		std::vector<std::weak_ptr<SDockingArea>> m_dock_areas;

		/*
		* collapsed dock areas refers to areas that were closed (e.g., by the user)
		* we save its location so they can be re-opened in the same location if the user opens thems again
		*/
		std::vector<std::shared_ptr<FTabManager::FArea>> m_collapsed_dock_areas;

		std::shared_ptr<FTabSpawner> m_nomed_tab_spawner;

		/*protected private api that must only be accessed by the docking framework internals*/
		std::shared_ptr<FPrivateApi> m_private_api;

		/*the name of the layout being used*/
		DoDoUtf8String m_active_layout_name;

		/*prevent or allow drag operation*/
		bool m_b_can_do_drag_operation;

		/*allow systems to dynamically hide tabs*/
		//note:in terms of tab types to hide tabs
		//todo:implement FNamePermissionList

		/*tabs which have been temporarily put in the a sidebar*/
		std::vector<std::weak_ptr<SDockTab>> m_temporarily_sidebared_tabs;
	};

	class FGlobalTabmanager : public FTabManager
	{
	public:

		static const std::shared_ptr<FGlobalTabmanager>& get();

		static std::shared_ptr<FGlobalTabmanager> New()
		{
			return std::make_shared<FGlobalTabmanager>();
		}

		/*
		* register a new nomad tab spawner with the tab manager, the spawner will be called when anyone calls
		* InvokeTab()
		* a nomad tab is a tab that can be placed with major tabs or minor tabs in any tab well
		* @param TabId the TabId to register the spawner for
		* @param OnSpawnTab the callback that will be used to spawn the tab
		* @param CanSpawnTab the callback that will be used to ask if spawning the tab is allowed
		* @return the registration entry for the spawner
		*/
		FTabSpawnerEntry& register_nomad_tab_spawner(const DoDoUtf8String& tab_id, const FOnSpawnTab& on_spawn_tab,
			const FCanSpawnTab& can_spawn_tab = FCanSpawnTab());

		FGlobalTabmanager()
			: FTabManager(std::shared_ptr<SDockTab>(), std::make_shared<FTabSpawner>())
		{}

		const DoDoUtf8String& get_application_title() const;

		void set_application_title(const DoDoUtf8String& app_title);//this title will set to main SWindow title

		/*provide a window under which all other windows in this application should nest*/
		void set_root_window(const std::shared_ptr<SWindow> in_root_window);

		/*the window under which all other windows in our app nest, might be null*/
		std::shared_ptr<SWindow> get_root_window() const;

		/*a window under which all of the windows in this application will nest*/
		std::weak_ptr<SWindow> m_root_window_ptr;

		DoDoUtf8String m_app_title;
	};
}