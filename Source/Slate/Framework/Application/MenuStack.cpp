#include "PreCompileHeader.h"

#include "MenuStack.h"

#include "SlateCore/Widgets/SCompoundWidget.h"

#include "SlateCore/Widgets/DeclarativeSyntaxSupport.h"

#include "SlateCore/Types/SlateStructs.h"//FOptionalSize depends on it

#include "Slate/Widgets/Layout/SBox.h"//SBox depends on it

#include "SlateCore/Widgets/SOverlay.h"//SOverlay depends on it

#include "SlateCore/Widgets/Images/SImage.h"//SImage depends on it

#include "Slate/Widgets/Layout/SBorder.h"

#include "SlateCore/Layout/WidgetPath.h"//FWidgetPath depends on it

#include "Menu.h"//FMenuBase depends on it

#include "Slate/Widgets/Layout/SPopup.h"//SPopup depends on it

namespace MenuStackInternal
{
	using namespace DoDo;
	/*
	* widget that wraps any menu created in FMenuStack to provide default key handling, focus tracking and helps us spot menus in widget paths
	*/
	DECLARE_DELEGATE_RetVal_OneParam(FReply, FOnKeyDown, FKey)
	class SMenuContentWrapper : public SCompoundWidget
	{
	public:
		SLATE_BEGIN_ARGS(SMenuContentWrapper)
			: _MenuContent()
			, _OnKeyDown()
			, _OptionalMinMenuWidth()
			, _OptionalMinMenuHeight()
		{}
		SLATE_DEFAULT_SLOT(FArguments, MenuContent) //note:this is menu
			SLATE_EVENT(FOnKeyDown, OnKeyDown)
			//SLATE_EVENT(FOnMenuLostFocus, OnMenuLostFocus)
			SLATE_ARGUMENT(FOptionalSize, OptionalMinMenuWidth)
			SLATE_ARGUMENT(FOptionalSize, OptionalMinMenuHeight)
		SLATE_END_ARGS()
		/*construct this widget*/
		void Construct(const FArguments& in_args)
		{
			//the visibility of the content wrapper should match that of the provided content

			//todo:set visibility

			m_on_key_down_delegate = in_args._OnKeyDown;

			m_child_slot
			[
				SNew(SBox)
				.MinDesiredWidth(in_args._OptionalMinMenuWidth)
				.MaxDesiredHeight(in_args._OptionalMinMenuHeight)
				[
					//always add a background to the menu, this includes a small outline around the background to distinguish open menus from each other
					SNew(SOverlay)
					+ SOverlay::Slot()
					[
						SNew(SImage)
						.Image(FCoreStyle::get().get_brush("Menu.Background"))
					]
					+ SOverlay::Slot()
					[
						SNew(SImage)
						.Image(FCoreStyle::get().get_brush("Menu.Outline"))//todo:implement get optional brush
					]
					+ SOverlay::Slot()
					[
						SNew(SBorder)
						.Padding(0.0f)
						.BorderImage(FStyleDefaults::get_no_brush())//todo:implement SBorder default foreground
						[
							in_args._MenuContent.m_widget
						]
					]
				]
			];
		}

	private:
		/*delegate to forward keys down events on the menu*/
		FOnKeyDown m_on_key_down_delegate;
	};


}

namespace DoDo {
	bool FMenuStack::has_menus() const
	{
		return m_stack.size() > 0;
	}
	std::shared_ptr<IMenu> FMenuStack::push(const FWidgetPath& in_owner_path, const std::shared_ptr<SWidget>& in_content, const glm::vec2& summon_location, const FPopupTransitionEffect& transation_effect, const bool b_focus_immeidately, const glm::vec2& summon_location_size, std::optional<EPopupMethod> in_method, const bool b_enable_per_pixel_transparency)
	{
		//we want to ensure that when the window is restored to restore the current keyboard focus
		//todo:implement SetWidgetToFocusOnActive

		FSlateRect anchor(summon_location, summon_location + summon_location_size);

		std::shared_ptr<IMenu> parent_menu;

		if (has_menus())
		{
			//find a menu in the stack in InOwnerPath to determine the level to insert this
			parent_menu = find_menu_in_widget_path(in_owner_path);
		}

		if (!parent_menu)
		{
			//pushing a new root menu (leave parent menu invalid)

			//the active method is determined when a new root menu is pushed
			
		}

		return nullptr;
	}
	std::shared_ptr<IMenu> FMenuStack::push_internal(const std::shared_ptr<IMenu>& in_parent_menu, const std::shared_ptr<SWidget>& in_content, FSlateRect anchor, const FPopupTransitionEffect& transition_effect, const bool b_focus_immeidately, EShouldThrottle should_throttle, const bool b_is_collapsed_by_parent, const bool b_enable_per_pixel_transparency)
	{
		FPrePushArgs pre_push_args;
		pre_push_args.m_content = in_content;
		pre_push_args.m_anchor = anchor;
		pre_push_args.m_transition_effect = transition_effect;
		pre_push_args.m_b_focus_immediately = b_focus_immeidately;
		pre_push_args.m_b_is_collapsed_by_parent = b_is_collapsed_by_parent;

		//pre-push stage
		//determines correct layout
		//wraps content
		//other common setup steps needed by all (non-hosted) menus
		const FPrePushResults pre_push_results = pre_push(pre_push_args);

		//menu object creation stage
		

		return nullptr;
	}
	FMenuStack::FPrePushResults FMenuStack::pre_push(const FPrePushArgs& in_args)
	{
		FPrePushResults out_results;

		out_results.m_b_is_collapsed_by_parent = in_args.m_b_is_collapsed_by_parent;
		out_results.m_b_focus_immediately = in_args.m_b_focus_immediately;
		if (in_args.m_b_focus_immediately)
		{
			out_results.m_widget_to_focus = in_args.m_content;
		}

		//only enable window position/size transitions if we're running at a decent frame rate
		out_results.m_b_allow_animations = false;

		//calc the max height available on screen for the menu
		float max_height;
		const float application_scale = 1.0f;//todo:get application scale

		//todo:check popup method reply

		bool b_anchor_sets_min_width = in_args.m_transition_effect.SlideDirection == FPopupTransitionEffect::ComboButton;

		//wrap menu content in a box needed for various sizing and tracking purposes
		FOptionalSize optional_min_width = b_anchor_sets_min_width ? in_args.m_anchor.get_size().x : FOptionalSize();
		FOptionalSize optional_min_height = max_height;

		//wrap content in a SPopup before the rest of the wrapping process - should make menus appear on top using deferred presentation
		std::shared_ptr<SWidget> temp_content = SNew(SPopup)[in_args.m_content];

		out_results.m_wrapped_content = wrap_content(temp_content, optional_min_width, optional_min_height);

		out_results.m_wrapped_content->slate_prepass(application_scale);

		out_results.m_expected_size = out_results.m_wrapped_content->get_desired_size() * application_scale;

		EOrientation orientation = (in_args.m_transition_effect.SlideDirection == FPopupTransitionEffect::SubMenu) ? Orient_Horizontal : Orient_Vertical;

		//calculate the correct position for the menu based on the popup method
		

		return out_results;
	}
	std::shared_ptr<SWidget> FMenuStack::wrap_content(std::shared_ptr<SWidget> in_content, FOptionalSize optional_min_width, FOptionalSize optional_min_height)
	{
		//wrap menu content in a box that limits its maximum height
		//and in a SMenuContentWrapper that handles key presses and focus change
		return SNew(MenuStackInternal::SMenuContentWrapper)
				.OptionalMinMenuHeight(optional_min_height)
				.OptionalMinMenuWidth(optional_min_width)
				.MenuContent()
				[
					in_content
				];
	}
	std::shared_ptr<IMenu> FMenuStack::find_menu_in_widget_path(const FWidgetPath& path_to_query) const
	{
		for (int32_t path_index = path_to_query.m_widgets.num() - 1; path_index >= 0; --path_index)
		{
			std::shared_ptr<const SWidget> widget = path_to_query.m_widgets[path_index].m_widget;

			auto it = m_cached_content_map.find(widget);
			if (it != m_cached_content_map.end())
			{
				const std::shared_ptr<FMenuBase> found_menu = it->second;
				return found_menu;
			}
		}

		return std::shared_ptr<IMenu>();
	}
}


