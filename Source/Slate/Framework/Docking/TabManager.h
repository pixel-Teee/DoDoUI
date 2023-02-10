#pragma once

#include <memory>

#include "Core/String/DoDoString.h"

#include "SlateCore/Types/SlateEnums.h"//EOrientation depends on it

#include "glm/glm.hpp"

namespace DoDo
{
	class SWindow;
	class SWidget;

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

	enum class ETabIdFlags : uint8_t
	{
		None = 0x0, //no flags
		SaveLayout = 0x1,//this tab should be included when saving the slate layout
	};

	struct FTabId
	{
		DoDoUtf8String m_tab_type;
		int32_t m_instance_id;

	private:
		ETabIdFlags m_flags;//note:need to save the tab?
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
			FTabId m_tab_id;//tab id, need save?
			ETabState::Type m_tab_state;//the tab state, open or close or sidebar?
			ESidebarLocation m_side_bar_location;//tab sidebar location
			float m_side_bar_size_coefficient;//bar size coefficient
			bool m_b_pinned_in_side_bar;//todo:how to understand it?
		};

		class FStack : public FLayoutNode
		{
			friend class FTabManager;

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

		protected:
			FSplitter()
			: m_orientation(Orient_Horizontal)
			{
				
			}

			EOrientation m_orientation;//have orientation
			std::vector<std::shared_ptr<FLayoutNode>> m_child_nodes;
		};

		class FArea : public FSplitter//FArea is inherited from FSplitter
		{
			friend class FTabManager;
		public:
			enum EWindowPlacement//todo:how to understand it?
			{
				Placement_NoWindow,
				Placement_Automatic,
				Placement_Specified
			};

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

		protected:
			std::weak_ptr<FArea> m_primary_area;
			std::vector<std::shared_ptr<FArea>> m_areas;
			/*the layout will be saved into a config file with this name, e.g. LevelEditorLayout or MaterialEditorLayout*/
			DoDoUtf8String m_layout_name;
		};

	public:
		std::shared_ptr<SWidget> restore_from(const std::shared_ptr<FLayout>& layout, const std::shared_ptr<SWindow>& parent_window,
		const bool b_embed_title_area_content = false, const EOutputCanBeNullptr restore_are_out_put_can_be_nullptr = EOutputCanBeNullptr::Never);//fourth parameter controls the return value
	protected:

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

	protected:
		/*the name of the layout being used*/
		DoDoUtf8String m_active_layout_name;
	};
}