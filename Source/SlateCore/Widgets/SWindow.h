#pragma once

#include "SCompoundWidget.h"

#include "ApplicationCore/GenericPlatform/GenericWindowDefinition.h"

#include "SlateCore/FastUpdate/SlateInvalidationRoot.h"//FSlateInvalidationContext
#include "SlateCore/Input/HittestGird.h"//FHittestGrid depends on it

#include "SlateCore/Widgets/SBoxPanel.h"//SVerticalBox depends on it

#include "SlateCore/Styling/SlateTypes.h"//FWindowStyle depends on it

#include "SlateCore/Styling/CoreStyle.h"//FCoreStyle depends on it

#include "SlateCore/Styling/ISlateStyle.h"//ISlateStyle depends on it

#include "SlateCore/Animation/CurveSequence.h"//FMorpher depends on it

namespace DoDo
{
	/*enum to descibe how windows are sized*/
	enum class ESizingRule : uint8_t
	{
		/*the windows size fixed and cannot be resized*/
		FixedSize,

		/*the window size is computed from its content and cannot be resized by users*/
		AutoSized,

		/*the window can be resized by users*/
		UserSized
	};

	/*proxy structure to handle deprecated construction from bool*/
	struct FWindowTransparency
	{
		FWindowTransparency(EWindowTransparency in) : m_value(in) {}

		EWindowTransparency m_value;
	};

	class Window;//native window
	class IWindowTitleBar;
	class SOverlay;
	class SImage;
	namespace SWindowDefs
	{
		/*height of a slate window title bar, in pixels*/
		static const float default_title_bar_size = 34.0f;
	}
	//todo:let SWindow to inherited from FSlateInvalidationRoot
	class SWindow : public SCompoundWidget, public FSlateInvalidationRoot
	{
	public:
		SLATE_BEGIN_ARGS(SWindow)
			: _Type(EWindowType::Normal)
			, _Style(&FCoreStyle::get().get_widget_style<FWindowStyle>("Window"))
			, _Title()
			, _ScreenPosition(glm::vec2(0.0f, 0.0f))
			, _ClientSize(glm::vec2(0.0f, 0.0f))
			, _AdjustInitialSizeAndPositionForDPIScale(true)
			, _SupportsTransparency(EWindowTransparency::None)
			, _InitialOpacity(1.0f)
			, _SizingRule(ESizingRule::UserSized)
			, _IsPopupWindow(false)
			, _CreateTitleBar(true)
			, _UserResizeBorder(FMargin(5, 5, 5, 5))
		{}
			/*type of this window*/
			SLATE_ARGUMENT(EWindowType, Type)
			/*style used to draw this window*/
			SLATE_STYLE_ARGUMENT(FWindowStyle, Style)
			/*title of the window*/
			SLATE_ATTRIBUTE(DoDoUtf8String, Title)
			/*screen-space position where the window should be initially located*/
			SLATE_ARGUMENT(glm::vec2, ScreenPosition)
			/*what the initial size of the window should be*/
			SLATE_ARGUMENT(glm::vec2, ClientSize)
			/*how to window should be sized*/
			SLATE_ARGUMENT(ESizingRule, SizingRule)
			/*true if this should be a 'pop-up' window*/
			SLATE_ARGUMENT(bool, IsPopupWindow)
			/*if the initial ClientSize and ScreenPosition arguments should be automatically adjusted to account for DPI scale*/
			SLATE_ARGUMENT(bool, AdjustInitialSizeAndPositionForDPIScale)

			/*should this window support transparency*/
			SLATE_ARGUMENT(FWindowTransparency, SupportsTransparency)

			/*the initial opacity of the window*/
			SLATE_ARGUMENT(float, InitialOpacity)
			/*
			* true if we should initially create a traditional title bar area
			* if false, the user must embed the title area content into the window manully, taking into account platform-specific considerations!
			* has no effect for certain types of windows (popups, tool-tips, etc)
			*/
			SLATE_ARGUMENT(bool, CreateTitleBar)

			/*the margin around the edges of the window that will be detected as places the user can grab to resize the window*/
			SLATE_ARGUMENT(FMargin, UserResizeBorder)

			SLATE_DEFAULT_SLOT(FArguments, Content)//declare a slot

		SLATE_END_ARGS()

		SWindow();

		//todo:implement FSlateWindowElementList FWidgetStyle 
		//int32_t paint_window(double current_time, float delta_time);

		virtual ~SWindow() {}

	public:
		void Construct(const FArguments& in_args);

		/*
		* make cursor decorator window
		* 
		* @return the new SWindow
		*/
		static std::shared_ptr<SWindow> make_cursor_decorator();

		/*
		* @param ContentSize the size of content that we want to accommodate
		* 
		* @return the size of the window necessary to accommodate the given content
		*/
		static glm::vec2 compute_window_size_for_cotent(glm::vec2 content_size);

		/*return true if the window is visible, false otherwise*/
		bool is_visible() const;

		/*
		* sets the opacity of this window
		* 
		* @param InOpacity the new window opacity represented as a floating point scalar
		*/
		void set_opacity(const float in_opacity);

		/*@return the window's current opacity*/
		float get_opacity() const;

		/*@return the level of transparency supported by this window*/
		EWindowTransparency get_transparency_support() const;

		/*
		* grabs the window type
		* 
		* @return the window's type
		*/
		EWindowType get_type() const
		{
			return m_type;
		}

		/*
		* grabs the current window title
		* 
		* @return the window's title
		*/
		DoDoUtf8String get_title() const
		{
			return m_title.Get();
		}

		void set_title_bar(const std::shared_ptr<IWindowTitleBar> in_title_bar)
		{
			m_title_bar = in_title_bar;
		}

		/*@return the geometry of the window in window space(i.e. position and absolute position are 0)*/
		FGeometry get_window_geometry_in_window() const;

		/*
		* sets the current window title
		* 
		* @param InTitle the new title of the window
		*/
		void set_title(const DoDoUtf8String& in_title)
		{
			m_title = in_title;
			if (m_native_window)
			{
				//todo:implement this
				//m_native_window->set_text()
			}
		}

		/*paint the window and all of it's contents, not the same as Paint()*/
		int32_t paint_window(double current_time, float delta_time, FSlateWindowElementList& out_draw_elements, const FWidgetStyle& in_widget_style, bool b_parent_enabled);//todo:implement FWidgetStyle

		std::shared_ptr<Window> get_native_window();

		/*@return the list of this window's child windows*/
		const std::vector<std::shared_ptr<SWindow>>& get_child_windows() const;

		std::vector<std::shared_ptr<SWindow>>& get_child_windows();

		/*return the parent of this window, invalid shared pointer if this window is not a child*/
		std::shared_ptr<SWindow> get_parent_window() const;

		/*
		 * returns the viewport size, taking into consideration if the window size should drive the viewport size
		 */
		glm::vec2 get_view_port_size() const
		{
			return (m_view_port_size.x) ? m_view_port_size : m_size;
		}

		/*@return true if the window accepts input, false if the window is non-interactive*/
		bool accepts_input() const;

		/*@return true if the window is sized by the windows content*/
		bool is_auto_sized() const;

		/*should this window automatically derive its size based on its content or be user-drive?*/
		void set_sizing_rule(ESizingRule in_sizing_rule);

		/*
		 * access the hittest acceleration data structure for this window
		 * the grid is filled out every time the window is painted
		 *
		 * @see FHittestGrid for more details
		 */
		FHittestGrid& get_hittest_grid();

		/*
		* @return the initially desired screen position of the slate window
		*/
		glm::vec2 get_initial_desired_size_in_screen() const;

		/*
		* @return the initially desired size of the slate window
		*/
		glm::vec2 get_initial_desired_position_in_screen() const;

		/*@return the desired size in desktop pixels*/
		glm::vec2 get_desired_size_desktop_pixels() const;

		/*
		 * @return the size of the window in screen space
		 */
		glm::vec2 get_size_in_screen() const;

		/* @return the position of the window in screen space */
		glm::vec2 get_position_in_screen() const;

		/*@return a clipping rectangle that represents this window in window space (i.e. alwyas starts at 0.0)*/
		FSlateRect get_clipping_rectangle_in_window() const;

		/*request that this window be destroyed, the window is not destroyed immediately, instead it is placed in a queue for destrution on next tick*/
		void request_destroy_window();

		/*use request destroy window whenever possible! this method destroys the window immediately*/
		void destroy_window_immediately();

		/*make the window visible*/
		void show_window();

		/*
		* sets the native os window associated with this SWindow
		* 
		* @param InNativeWindow the native window
		*/
		void set_native_window(std::shared_ptr<Window> in_native_window);

		/*sets the actual screen position of the window, this should only be called by the os*/
		void set_cached_screen_position(glm::vec2 new_position);

		/*sets the actual screen position of the window, this should only be called by the os*/
		//note:called by the os

		void set_cached_size(glm::vec2 new_size);

		/*relocate the window to a screenspace position specified by new position*/
		void move_window_to(glm::vec2 new_position);

		/*relocate the window to a screenspace position specified by new position and resize it to new size*/
		void reshape_window(glm::vec2 new_position, glm::vec2 new_size);
		void reshape_window(const FSlateRect& in_new_shape);

		/*set a new morph shape and force the morph to run for at least one frame in order to reach that target*/
		void update_morph_target_shape(const FSlateRect& target_shape);

		/*
		* resize the window to be dpi scaled new client size immediately
		* 
		* @param new client size: client size with dpi scaling already applied that does not include border or title bars
		*/
		void resize(glm::vec2 new_client_size);

		/*gets the current window zone that mouse position is over*/
		EWindowZone::Type get_current_window_zone(glm::vec2 local_mouse_position);

		/*used to store the zone where the mouse down event occurred during move/drag*/
		EWindowZone::Type m_move_resize_zone;

		/*
		 * sets the widget content for this window
		 *
		 * @param InContent the widget to use as content for this window
		 */
		void set_content(std::shared_ptr<SWidget> in_content);

		/*@return true if mouse coordinates is within this window*/
		bool is_screen_space_mouse_within(glm::vec2 screen_space_mouse_coordinate) const;

		/*resize using already dpi scaled window size including borders/title bar*/
		void resize_window_size(glm::vec2 new_window_size);
	public:
		/*
		* for a given client size, calculate the window size required to accommodate any potential non-os borders and title bars
		* 
		* @param InClientSize: client size with dpi scaling already applied
		* @param DPIScale: scale that will be applied for border and title, when not supplied detects dpi scale using native or initial position
		*/
		glm::vec2 get_window_size_from_client_size(glm::vec2 in_client_size, std::optional<float> dpi_scale = std::optional<float>());
	private:
		virtual FReply On_Mouse_Button_On_Down(const FGeometry& my_geometry, const FPointerEvent& mouse_event) override;

		virtual FReply On_Mouse_Button_On_Up(const FGeometry& my_geometry, const FPointerEvent& mouse_event) override;

		virtual FReply On_Mouse_Move(const FGeometry& my_geometry, const FPointerEvent& mouse_event) override;

		virtual int32_t On_Paint(const FPaintArgs& args, const FGeometry& allotted_geometry, const FSlateRect& my_culling_rect, FSlateWindowElementList& out_draw_elements,
			int32_t layer_id, const FWidgetStyle& in_widget_style, bool b_parent_enabled) const override;

		/*the window's desired size takes into account the ratio between the slate units and the pixel size*/
		virtual glm::vec2 Compute_Desired_Size(float) const override;
	protected:
		/*type of the window*/
		EWindowType m_type;

		/*how to size the window*/
		ESizingRule m_sizing_rule;

		/*title of the window, displayed in the title bar as well as potentially in the task bar (windows platform)*/
		TAttribute<DoDoUtf8String> m_title;

		/*current opacity of the window*/
		float m_opacity;

		/*transparency setting for this window*/
		EWindowTransparency m_transparency_support;

		/*true if this window has a title bar*/
		bool b_create_title_bar : 1;

		/*true if this is a pop up window*/
		bool m_b_is_pop_up_window : 1;

		/*true if this window has been shown yet*/
		bool m_b_has_ever_been_shown : 1;

		/*initial desired position of the window's content in screen space*/
		glm::vec2 m_initial_desired_screen_position;

		/*initial desired position of the window's content in screen space*/
		glm::vec2 m_initial_desired_size;

		/*position of the window's content in screen position*/
		glm::vec2 m_screen_position;

		/*size of the window's content area in screen space*/
		glm::vec2 m_size;

		/*size of the viewport, if (0, 0) then it is equal to size*/
		glm::vec2 m_view_port_size;

		/*size of this window's title bar, can be zero, set at construction and should not be changed afterwards*/
		float m_title_bar_size;

		/*utility for animating the window size*/
		struct FMorpher
		{
			FMorpher()
				: m_starting_morph_shape(FSlateRect(0, 0, 100, 100))
				, m_target_morph_shape(FSlateRect(0, 0, 100, 100))
				, m_b_active(false)
				, m_b_is_animating_window_size(false)
			{}
			/*initial window opacity*/
			float m_starting_opacity;

			/*desired opacity of the window*/
			float m_target_opacity;

			/*initial size of the window(i.e. at the start of animation)*/
			FSlateRect m_starting_morph_shape;

			/*desired size of the window(i.e. at the end of animation)*/
			FSlateRect m_target_morph_shape;

			/*animation sequence to hold on to the handle*/
			FCurveSequence m_sequence;

			/*true if this morph is currently active*/
			bool m_b_active : 1;

			/*true if we're morphing size as well as position, false if we're just morphing position*/
			bool m_b_is_animating_window_size : 1;
		} Morpher;

		/*cached "zone" the cursor was over in the window the last time that someone called get_current_window_zone()*/
		EWindowZone::Type m_window_zone;

		SVerticalBox::FSlot* m_content_slot;
	private:
		/*style used to draw this window*/
		const FWindowStyle* m_style;

		const FSlateBrush* m_window_background;

		std::shared_ptr<SImage> m_window_background_image;

		std::shared_ptr<SWidget> m_content_area_v_box;

		EVisibility m_window_content_visibility;
	protected:
		/*the native window that is backing this slate window*/
		//todo:implement FGenericWindow
		std::shared_ptr<Window> m_native_window;

		/*each window has it's own hittest grid for accelerated widget picking*/
		std::unique_ptr<FHittestGrid> m_hittest_grid;

		/*window overlay widget*/
		std::shared_ptr<SOverlay> m_window_overlay;

		/*when not null, this window will always appear on top of the parent and be closed when the parent is closed*/
		std::weak_ptr<SWindow> m_parent_window_ptr;

		/*child windows of this window*/
		std::vector<std::shared_ptr<SWindow>> m_child_windows;

		//the window title bar
		std::shared_ptr<IWindowTitleBar> m_title_bar;

		//the margin around the edges of the window that will be detected as places the user can grab to resize the window
		FMargin m_user_resize_border;

	protected:

		void construct_window_internals();

		void update_window_content_visibility();

		//------FSlateInvalidationRoot overrides------
		virtual int32_t paint_slow_path(const FSlateInvalidationContext& context) override;
		//------FSlateInvalidationRoot overrides------
	};
}
