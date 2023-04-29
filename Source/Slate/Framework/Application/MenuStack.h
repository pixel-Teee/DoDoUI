#pragma once

#include "SlateCore/Layout/SlateRect.h"//FSlateRect depends on it

#include "SlateCore/Input/PopupMethodReply.h"//EPopupMethod and EShouldThrottle depends on it

#include "SlateCore/Types/SlateStructs.h"//FOptionalSize

namespace DoDo {
	/*describes a simple animation for popup window introductions*/
	struct FPopupTransitionEffect
	{
		/*direction to slide in from*/
		enum ESlideDirection
		{
			/*no sliding*/
			None,

			/*sliding direction for a combo button*/
			ComboButton,

			/*sliding direction for a top-level pull-down menu or combo box*/
			TopMenu,

			/*sliding direction for a sub-menu*/
			SubMenu,

			/*sliding direction for a popup that lets the user type in data*/
			TypeInPopup,

			/*sliding direction preferred for context menu popups*/
			ContextMenu

		} SlideDirection;

		/*constructor*/
		FPopupTransitionEffect(const ESlideDirection init_slide_direction)
			: SlideDirection(init_slide_direction)
		{}
	};
	class IMenu;
	class FMenuBase;
	class SWidget;
	class FWidgetPath;
	//enum class EPopupMethod : uint8_t;
	/*
	* represents a stack of open menus
	* the last item in the stack is the top most menu
	* menus are described as IMenus, implementations of IMenu can control how the menus are created and presented (e.g. in their own window)
	*/
	class FMenuStack
	{
	public:
		/*constructor*/
		FMenuStack()
			: m_b_host_window_guard(false)
		{

		}

		/*contains all the options passed to the pre-push stage of the menu creation process*/
		struct FPrePushArgs
		{
			FPrePushArgs() : m_transition_effect(FPopupTransitionEffect::None) {}

			std::shared_ptr<SWidget> m_content;
			FSlateRect m_anchor;
			FPopupTransitionEffect m_transition_effect;
			bool m_b_focus_immediately;
			bool m_b_is_collapsed_by_parent;
		};

		/*contains all the options returned from the pre-push stage of the menu creation process*/
		struct FPrePushResults
		{
			std::shared_ptr<SWidget> m_wrapped_content;
			std::shared_ptr<SWidget> m_widget_to_focus;
			glm::vec2 m_anim_start_location;
			glm::vec2 m_anim_final_location;
			bool m_b_anchor_sets_min_width;
			glm::vec2 m_expected_size;
			bool m_b_allow_animations;
			bool m_b_focus_immediately;
			bool m_b_is_collapsed_by_parent;
		};

		/*
		* @return true if the stack has one or more menus in it, false if it is empty
		*/
		bool has_menus() const;

		/*
		* pushes a new menu onto the stack, the widget path will be searched for existing menus and the new menu will be parented appropriately
		* menus are always auto-sized, use fixed-size content if a fixed size is required
		* 
		* @param InOwnerPath the widget path for the parent widget of this menu
		* @param InContent the menu's content
		* @param SummonLocation location in screen-space where the menu should appear
		* @param TransitionEffect animation to use when the popup appears
		* @param bFocusImmediately should the popup steal focus when shown?
		* @param SummonLocationSize an optional size around the summon location which describes an area in which the menu may not appear
		* @param InMethod an optional popup method that will override the default method for the widgets in InOwnerPath
		* @param bIsCollapsedByParent is this menu collapsed when a parent menu receives focus/activation? if false, only focus/activation outside the entire stack will auto collapse it
		* @param bEnablePerPixelTransparency does the menu's content require per pixel transparency?
		*/
		std::shared_ptr<IMenu> push(const FWidgetPath& in_owner_path, const std::shared_ptr<SWidget>& in_content, const glm::vec2& summon_location, const FPopupTransitionEffect& transation_effect,
			const bool b_focus_immeidately = true, const glm::vec2& summon_location_size = glm::vec2(0.0f), std::optional<EPopupMethod> in_method = std::optional<EPopupMethod>(), const bool b_enable_per_pixel_transparency = false);

		/*
		* this is the common code used during menu creation, this stage is not used by "hosted" menus
		* it handles pre-push, actual menu creation and post-push stack updates
		* 
		* @param InParentMenu the parent menu for this menu
		* @param InContent the menu's content
		* @param Anchor the anchor location for the menu (rect around the parent widget that is summoning the menu)
		* @param TransitionEffect animation to use when the popup appears
		* @param bFocusImmediately should the popup steal focus when shown?
		* @param bIsCollapsedByParent is this menu collapsed when a parent menu receives focus/activation? if false, only focus/activation outside the entire stack will auto collapsed it
		* 
		* @return the newly created menu
		*/
		std::shared_ptr<IMenu> push_internal(const std::shared_ptr<IMenu>& in_parent_menu, const std::shared_ptr<SWidget>& in_content, FSlateRect anchor,
			const FPopupTransitionEffect& transition_effect, const bool b_focus_immeidately, EShouldThrottle should_throttle, const bool b_is_collapsed_by_parent, const bool b_enable_per_pixel_transparency);

		/*
		* this is the pre-push stage of menu creation
		* it is responsible for wrapping content and calculating menu positioning, this stage is not used by "hosted" menus
		* 
		* @param InArgs pre-push options
		* 
		* @return pre-push results
		*/
		FPrePushResults pre_push(const FPrePushArgs& in_args);

		/*
		* called by all menus during creation, handles wrapping content in SMenuContentWrapper that acts as a  marker in widget paths and in a box that sets minimum size
		* 
		* @param InContent the unwrapped content
		* @param OptionalMinWidth optional minimum width for the wrapped content
		* @param OptionalMinHeight optional minimum height for the wrapped content
		*/
		std::shared_ptr<SWidget> wrap_content(std::shared_ptr<SWidget> in_content, FOptionalSize optional_min_width = FOptionalSize(), FOptionalSize optional_min_height = FOptionalSize());

		/*
		* searches from bottom to top of the widget path for a menu in the stack
		* if the widget at the end of PathToQuery is inside a menu, there will be a menu content wrapper widget in the path
		* 
		* @param PathToQuery the widget path to search for a menu
		* @return the menu in the stack that contains the widget at the end of PathToQuery, or an invalid ptr if not found
		*/
		std::shared_ptr<IMenu> find_menu_in_widget_path(const FWidgetPath& path_to_query) const;

		/*guard to prevent the host window and host window popup panel being set reentrant*/
		bool m_b_host_window_guard;

		/*the array of menus in the stack*/
		std::vector<std::shared_ptr<FMenuBase>> m_stack;//note:FMenuBase inherited from IMenu

		/*maps top-level content widgets (should always be SMenuContentWrappers) to menus in the stack*/
		std::map<std::shared_ptr<const SWidget>, std::shared_ptr<FMenuBase>> m_cached_content_map;
	};
}