#pragma once

#include "Core/Core.h"

#include "Core/Delegates/Delegates.h"

#include <vector>

#include "SlateCore/Layout/WidgetPath.h"//FWidgetPath
//#include "SlateCore/Widgets/SWindow.h"

#include "ApplicationCore/GenericPlatform/GenericApplicationMessageHandler.h"

#include "Slate/Framework/SlateUser.h"//FSlateUser depends on it

#include "SlateCore/Input/Reply.h"//FReply depends on it

#include <functional>//std::function<void(FSlateUser&)> depends on it

#include "SlateCore/Styling/SlateColor.h"//FSlateColor depends on it
#include "SlateCore/Types/SlateEnums.h"//EOrientation depends on it

namespace DoDo
{
	class RendererInstance;

	class Window;

	class SWindow;

	class GenericApplication;//platform application

	class Renderer;

	struct FPointerEvent;

	class STextBlock;//for test
	class SBorder;//for test
	class SImage;
	struct FSlateBrush;

	struct FWindowTitleBarArgs;
	class IWindowTitleBar;
	class Application;
	/*
	 * private interface to control which classes are allowed to perform hit-testing
	 */
	class FHitTesting
	{
	public:
		FHitTesting(Application* in_slate_application)
			: m_slate_app(in_slate_application)
		{}

	private:
		//SWindow must be able to test which part of the window is being moused-over
		friend class SWindow;

	private:
		Application* m_slate_app;

		//FSlateApplicationBase::LocateWidgetInWindow
		FWidgetPath locate_widget_in_window(glm::vec2 screen_space_mouse_coordinate, const std::shared_ptr<SWindow>& window, bool b_ignore_enabled_status,
			int32_t user_index) const;
	};

	class Application : public FGenericApplicationMessageHandler//slate application
	{
	public:
		Application();

		~Application();

		static std::shared_ptr<SWindow> test_create_widget();
		static bool make_platform_window_and_create_view_port(std::shared_ptr<SWindow>);

		static void Create();

		static void shut_down();

		static std::shared_ptr<Application> Create(const std::shared_ptr<GenericApplication>& in_platform_application);

		static void initialize_core_style();

		/*
		 * gets the renderer being used to draw this application
		 * @return the slate renderer
		 */
		Renderer* get_renderer() const
		{
			return m_renderer.get();
		}

		void Initialize_Renderer(std::shared_ptr<Renderer> in_renderer);

		std::shared_ptr<FSlateUser> get_user(int32_t user_index)
		{
			return (user_index >= 0 && user_index < m_users.size()) ? m_users[user_index] : nullptr;
		}

		std::shared_ptr<const FSlateUser> get_user(int32_t user_index) const
		{
			return (user_index >= 0 && user_index < m_users.size()) ? m_users[user_index] : nullptr;
		}

		/*get the standard 'default' user (there's always guaranteed to be at least one)*/
		std::shared_ptr<const FSlateUser> get_cursor_user() const
		{
			std::shared_ptr<const FSlateUser> slate_user = get_user(m_cursor_user_index);//m_cursor_user_index is 0, to identify

			return slate_user;
		}

		std::shared_ptr<FSlateUser> get_cursor_user()
		{
			std::shared_ptr<FSlateUser> slate_user = get_user(m_cursor_user_index);//m_cursor_user_index is 0, to identify

			return slate_user;
		}

		/*
		* gets the size of the cursor
		* 
		* @return cursor size
		*/
		virtual glm::vec2 get_cursor_size() const;

		virtual glm::vec2 get_cursor_pos() const override;

		virtual glm::vec2 get_last_cursor_pos() const override;

		std::shared_ptr<ICursor> get_platform_cursor()
		{
			return s_platform_application->m_cursor;
		}

		/*
		* calculates the popup window position from the passed in window position and size
		* adjusts position for popup windows which are outside of the work area of the monitor where they reside
		* 
		* @param InAnchor the current(suggested) window position and size of an area which may not be covered by the popup
		* @param InSize the size of the window
		* @param InProposedPlacement the location on screen where the popup should go if allowed, if zero this will be determined from Orientation and Anchor
		* @param Orientation the direction of the popup
		*			if vertical it will attempt to open below the anchor but will open above if there is no room
		*			if horizontal it will attempt to open below the anchor but will open above if there is no room
		* 
		* @return the adjusted position
		*/
		virtual glm::vec2 calculate_popup_window_position(const FSlateRect& in_anchor, const glm::vec2& in_size, bool b_auto_adjust_for_dpi_scale = true,
			const glm::vec2& in_proposed_placement = glm::vec2(0.0f), const EOrientation orientation = Orient_Vertical) const;

		/*
		* calculates the tooltip window position
		* 
		* @param InAnchorRect the current(suggested) window position and size of an area which may not be covered by the popup
		* @param InSize the size of the tooltip window
		* @return the suggested position
		*/
		virtual glm::vec2 calculate_tooltip_window_position(const FSlateRect& in_anchor_rect, const glm::vec2& in_size, bool b_auto_adjust_for_dpi_scale) const;

		//void Run();
		void Tick();

		//Scope<Window> get_window();

		Window& get_window();//todo:remove this

		/*event before slate application ticks*/
		using FSlateTickEvent = Delegate_Event<void(float)>;
		FSlateTickEvent& on_pre_tick() { return m_pre_tick_event; }

		/*event after slate application ticks*/
		FSlateTickEvent& on_post_tick() { return m_post_tick_event; }

		void destroy_renderer();

		/*allows you do some operations for every registered user*/
		void for_each_user(std::function<void(FSlateUser&)> in_prediacte, bool b_include_virtual_users = false);

		/*
		 * locates the slate user object corresponding to the index, creating a new one if it doesn't exist
		 * asserts if given an invalid (ie, negative) index
		 */
		std::shared_ptr<FSlateUser> get_or_create_user(int32_t user_index);

		/*
		 * locates the slate user object corresponding to the index, creating a new one if it doesn't exist
		 * asserts if given an invalid (ie, negative) index
		 */
		std::shared_ptr<FSlateUser> get_or_create_user(const FInputEvent& input_event) { return get_or_create_user(input_event.get_user_index()); }

		/*
		 * register a new user with slate, normally this is unnecessary as slate automatically adds
		 * a user entry if it gets input from a controller for that index, might happed if the user
		 * allocates the virtual user
		 */
		std::shared_ptr<FSlateUser> register_new_user(int32_t user_index);

		/*transforms a pointer event to account for non-standard viewport resolutions*/
		FPointerEvent transform_pointer_event(const FPointerEvent& pointer_event, const std::shared_ptr<SWindow>& window) const;

		/*advances time for the application*/
		void tick_time();

		/*
		* pmups and ticks the platform
		*/
		void tick_platform(float delta_time);

		/*
		 * ticks and paints the actual slate portion of the application
		 */
		void Tick_And_Draw_Widgets(float delta_time);

		/*
		 * returns the real time delta since slate last ticked widgets
		 * @return the time delta since last tick
		 */
		const float get_delta_time() const { return (float)(m_current_time - m_last_tick_time); }

		/* draws slate windows. should only be called by the application's main loop or renderer */
		void Draw_Windows();

		/*
		 * draw slate windows, optionally only drawing the passed in window
		 */
		void Private_Draw_Windows(std::shared_ptr<SWindow> draw_only_this_window = nullptr);

		/*
		 * pre-pass step before drawing windows to compute geometry size and reshape autosized windows
		 */
		void Draw_Pre_Pass(std::shared_ptr<SWindow> draw_only_this_window);

		/*
		 * draws a window and it's children
		 */
		//todo:implement FDrawWindowAndArgs
		void Draw_Window_And_Children(const std::shared_ptr<SWindow>& window_to_draw, struct FDrawWindowArgs& draw_window_args);

		virtual FWidgetPath locate_widget_in_window(glm::vec2 screen_space_mouse_coordinate, const std::shared_ptr<SWindow>& window, bool b_ignore_enabled_status,
			int32_t user_index) const;

		/* @return an array of top-level windows that can be interacted with e.g. when a modal window is up, only return the modal window*/
		std::vector<std::shared_ptr<SWindow>> get_interactive_top_level_windows();

		/*
		* assign a delegate to be called when this application is requesting an exit (e.g. when the last window is closed)
		* 
		* @param OnExitRequestHandler function to execute when the application wants to quit
		*/
		void set_exit_requested_handler(const FSimpleDelegate& on_exit_requested_handler);

		/*
		 * sets up any values that need to be based on the physical dimensions of the device
		 * such as dead zone associated with precise tapping ... etc
		 */
		void set_up_physical_sensitivities();
	public:
		/*
		 * called by the native application in response to a mouse move, routs the event to slate widgets
		 * @param InMouseEvent Mouse Event
		 * @param bISynthetic true when the event is synthesized by slate
		 * @return was this event handled by the slate application?
		 */
		bool process_mouse_move_event(const FPointerEvent& mouse_event, bool b_is_synthetic = false);

		/*
		 * called by the native application in response to a mouse button press, routs the event to slate widgets
		 *
		 * @param PlatformWindow the platform window the event originated form, used to set focus at the platform level
		 *						 if invalid the mouse event will work but there will be no effect on the platform
		 * @param InMouseEvent mouse event
		 * @return was this event handled by the slate application?
		 */
		bool process_mouse_button_down_event(const std::shared_ptr<Window>& platform_window, const FPointerEvent& in_mouse_event);

		/*
		 * called by the native application in response to a mouse button release, routs the event to slate widgets
		 *
		 * @param InMouseEvent mouse event
		 * @return was this event handled by the slate application?
		 */
		bool process_mouse_button_up_event(const FPointerEvent& mouse_event);

		/*
		* returns the current instance of the application, the application should have been initialized before
		* this method is called
		* 
		* @return reference to the application
		*/
		static Application& get()
		{
			return *s_current_application;//slate application
		}

		/*
		* returns true if a slate application instance is currently intialized and ready
		* 
		* @return true if slate application is initialized
		*/
		static bool is_initialized()
		{
			return s_current_application != nullptr;
		}

		/*
		* gets the global application icon
		* 
		* @return the icon
		*/
		virtual const FSlateBrush* get_app_icon() const;
	public:
		/*
		* destroy the native and slate windows in the array provided
		* 
		* @param WindowsToDestroy destroy these windows
		*/
		void destroy_windows_immediately();
		/*
		* apply any requests from the reply to the application, e.g. capture mouse
		* 
		* @param CurrentEventPath the widget path along which the reply-generated event was routed
		* @param TheReply the reply generated by an event that was being processed
		* @param WidgetsUnderMouse optional widgets currently under the mouse, initiating drag and drop needs access to widgets under the mouse
		* @param InMouseEvent optional mouse event that caused this action
		* @param UserIndex user index that generated the event we are replying to (defaults to 0, at least for now)
		*/
		void process_reply(const FWidgetPath& current_event_path, const FReply& the_reply, const FWidgetPath* widgets_under_mouse, const FPointerEvent* in_mouse_event, const int32_t m_user_index = 0);

		/*@return the deadzone size for dragging in screen pixels(aka virtual desktop pixels)*/
		float get_drag_trigger_distance() const;
	public:
		//------------------------FGenericApplicationMessageHandler Interface------------------------
		virtual void set_cursor_pos(const glm::vec2 mouse_coordinate) override;

		virtual bool On_Mouse_Move() override;

		virtual bool On_Mouse_Move(double x, double y) override;

		virtual bool On_Mouse_Down(const std::shared_ptr<Window>& window, const EMouseButtons::Type button) override;

		virtual bool On_Mouse_Down(const std::shared_ptr<Window>& window, const EMouseButtons::Type button, const glm::vec2 cursor_pos) override;

		virtual bool On_Mouse_Up(const EMouseButtons::Type button) override;

		virtual bool On_Mouse_Up(const EMouseButtons::Type button, const glm::vec2 cursor_pos) override;

		virtual bool On_Size_Changed(const std::shared_ptr<Window>& window, const int32_t width, const int32_t height, bool b_was_minimized = false);

		virtual bool On_Cursor_Set() override;

		virtual EWindowZone::Type get_window_zone_for_point(const std::shared_ptr<Window>& platform_window, const int32_t x, int32_t y) override;

		//------------------------FGenericApplicationMessageHandler Interface------------------------

		/*
		* returns the current modifier keys state
		* 
		* @return state of modifier keys
		*/
		FModifierKeyState get_modifier_keys() const;

		const std::set<FKey>& get_pressed_mouse_buttons() const;
	public:
		const static uint32_t m_cursor_pointer_index;

		const static uint32_t m_cursor_user_index;
	public:
		//a black hole for android window
		std::shared_ptr<SWindow> get_first_window();

		std::shared_ptr<SWindow> add_window(std::shared_ptr<SWindow> in_slate_window, const bool b_show_immediately = true);

		std::shared_ptr<Window> make_window(std::shared_ptr<SWindow> in_slate_window, const bool b_show_immediately);

		/*
		* destroy an SWindow, removing it and all its children from the slate window list, notifies the native window to destroy itself and releases rendering resources
		* 
		* @param DestroyedWindow the window to destroy
		*/
		void private_destroy_window(const std::shared_ptr<SWindow>& destroyed_window);
	public:

		/*@return a hittesting object that can perform hittests agains widgets, only certain classes can make sure of FHitTesting*/
		friend class FHitTesting;
		const FHitTesting& get_hit_testing() const;

		virtual FWidgetPath locate_window_under_mouse(glm::vec2 screen_space_mouse_coordinate, const std::vector<std::shared_ptr<SWindow>>& windows,
			bool b_ignore_enabled_status = false, int32_t user_index = -1);

		/*
		 * directly routes a pointer down event to the widgets in the specified widget path
		 *
		 * @param widgets under pointer the path of widgets the event is routed to
		 * @param pointer event the event data that is routed to the widget path
		 *
		 * @return the reply returned by the widget that handled the event
		 */
		FReply route_pointer_down_event(const FWidgetPath& widgets_under_pointer, const FPointerEvent& pointer_event);

		/*
		 * directly routes a pointer up event to the widgets in the specified widget path
		 *
		 * @param widgets under pointer the path of widgets the event is routed to
		 * @param pointer event the event data that it is routed to the widget path
		 *
		 * @return the reply from the event
		 */
		FReply route_pointer_up_event(const FWidgetPath& widgets_under_pointer, const FPointerEvent& pointer_event);

		/*
		 * directly routes a pointer move event to the widgets in the specified widget path
		 *
		 * @param WidgetUnderPointer the path of widgets the event is routed to
		 * @param PointerEvent the event data that is routed to the widget path
		 * @param bIsSynthetic whether or not the move event is synthetic, synthetic pointer moves used simulate an event without the pointer actually moving
		 */
		bool route_pointer_move_event(const FWidgetPath& widgets_under_pointer, const FPointerEvent& pointer_event, bool b_is_synthetic);

		virtual bool does_widget_have_mouse_capture(const std::shared_ptr<const SWidget> widget) const;

		/*
		* gets whether or not a particular widget has mouse capture by a user
		* 
		* @return true if the widget has mouse capture, otherwise false
		*/
		virtual bool does_widget_have_mouse_capture_by_user(const std::shared_ptr<const SWidget> widget, int32_t user_index, std::optional<int32_t> pointer_index) const;

		virtual std::shared_ptr<SWidget> make_window_title_bar(const FWindowTitleBarArgs& in_args, std::shared_ptr<IWindowTitleBar>& out_title_bar) const;

		/*
		* destroying windows has implications on some OSs (e.g. destroying win32 hwnds can cause events to be lost)
		* 
		* slate strictly controls when windows are destroyed
		* 
		* @param WindowToDestroy the window to queue for destruction
		*/
		virtual void request_destroy_window(std::shared_ptr<SWindow> window_to_destroy);

		/*
		* creates an image widget
		* 
		* @return the new image widget
		*/
		virtual std::shared_ptr<SImage> make_image(const TAttribute<const FSlateBrush*>& image, const TAttribute<FSlateColor>& color, const TAttribute<EVisibility>& visibility) const;
	protected: 
		//holds the slate renderer used to render this application
		std::shared_ptr<Renderer> m_renderer;

		//private interface for select entities that are allowed to perform hittetsting
		FHitTesting m_hit_testing;

		//holds a pointer to the current slate application
		static std::shared_ptr<Application> s_current_application;

		std::set<FKey> m_pressed_mouse_buttons;//note:what it is?

		//holds a pointer to the platform application
		static std::shared_ptr<GenericApplication> s_platform_application;
	private:

		/*delegate for pre slate tick*/
		FSlateTickEvent m_pre_tick_event;

		/*delegate for post slate tick*/
		FSlateTickEvent m_post_tick_event;

		/*
		* provides a platform-agnostic method for rquesting that the application exit
		* implementations should assign a handler that terminates the process when this delegate is invoked
		*/
		FSimpleDelegate m_on_exit_requested;

		/* the current cached absolute real time, right before we tick widgets */
		double m_current_time;//todo:use platform current time to initialize

		/* last absolute real time that we ticked */
		double m_last_tick_time;

		/* running average time in seconds between calls to tick(used for monitoring responsiveness) */
		float m_average_delta_time;

		//void CreateInstance();

		Scope<RendererInstance> m_renderer_instance;

		Scope<Window> m_p_window;

		/*the dead zone distance in virtual desktop pixel (a.k.a screen pixels) that the user has to move their finder before it is considered a drag*/
		float m_drag_trigger_distance;

		//all top-level windows owned by this application, there are tracked here in a platform-agnostic way
		std::vector<std::shared_ptr<SWindow>> m_windows;

		/*these windows will be destroyed next tick*/
		std::vector<std::shared_ptr<SWindow>> m_window_destroy_queue;

		/*
		 * all users currently registered with slate
		 * normally there's just one, but any case where multiple users can provide input to the same application will register multiple users
		 *
		 * note: the array my contain invalid entries, users have associated indices that they expect to maintain, independent of the existence of other users
		 */
		std::vector<std::shared_ptr<FSlateUser>> m_users;

		//------calculate frame per second------
		DoDoUtf8String calculate_frame_per_second() const;

		int64_t m_frame_count = 0;
		int64_t m_last_frame_count = 0;

		double m_last_time = 0.0f;

		void test_slider_value_changed(float new_value);

		std::shared_ptr<STextBlock> m_text_block;

		std::shared_ptr<SBorder> m_border;

		void test_color_wheel_value_changed(FSlateColor new_color);
		//------calculate frame per second------

		//------test create sdock------
		void test_create_sdock();
		//------test create sdock------
	};

}
