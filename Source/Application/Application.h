#pragma once

#include "Core/Core.h"

#include "Core/Delegates/Delegates.h"

#include <vector>

#include "SlateCore/Layout/WidgetPath.h"//FWidgetPath
//#include "SlateCore/Widgets/SWindow.h"

#include "ApplicationCore/GenericPlatform/GenericApplicationMessageHandler.h"

#include "Slate/Framework/SlateUser.h"//FSlateUser depends on it

#include "Slate/Widgets/Input/Reply.h"//FReply depends on it

namespace DoDo
{
	class RendererInstance;

	class Window;

	class SWindow;

	class GenericApplication;//platform application

	class Renderer;

	struct FPointerEvent;

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

		virtual glm::vec2 get_cursor_pos() const override;

		virtual glm::vec2 get_last_cursor_pos() const override;

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
	public:
		//------------------------FGenericApplicationMessageHandler Interface------------------------
		virtual void set_cursor_pos(const glm::vec2 mouse_coordinate) override;

		virtual bool On_Mouse_Move() override;

		virtual bool On_Mouse_Down(const std::shared_ptr<Window>& window, const EMouseButtons::Type button) override;

		virtual bool On_Mouse_Down(const std::shared_ptr<Window>& window, const EMouseButtons::Type button, const glm::vec2 cursor_pos) override;

		virtual bool On_Mouse_Up(const EMouseButtons::Type button) override;

		virtual bool On_Mouse_Up(const EMouseButtons::Type button, const glm::vec2 cursor_pos) override;

		//------------------------FGenericApplicationMessageHandler Interface------------------------
	public:
		const static uint32_t m_cursor_pointer_index;

		const static uint32_t m_cursor_user_index;
	public:
		//a black hole for android window
		std::shared_ptr<SWindow> get_first_window();

		std::shared_ptr<SWindow> add_window(std::shared_ptr<SWindow> in_slate_window, const bool b_show_immediately = true);

		std::shared_ptr<Window> make_window(std::shared_ptr<SWindow> in_slate_window, const bool b_show_immediately);
	public:

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

	protected:
		//holds the slate renderer used to render this application
		std::shared_ptr<Renderer> m_renderer;

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

		/* the current cached absolute real time, right before we tick widgets */
		double m_current_time;//todo:use platform current time to initialize

		/* last absolute real time that we ticked */
		double m_last_tick_time;

		/* running average time in seconds between calls to tick(used for monitoring responsiveness) */
		float m_average_delta_time;

		//void CreateInstance();

		Scope<RendererInstance> m_renderer_instance;

		Scope<Window> m_p_window;

		//all top-level windows owned by this application, there are tracked here in a platform-agnostic way
		std::vector<std::shared_ptr<SWindow>> m_windows;

		/*
		 * all users currently registered with slate
		 * normally there's just one, but any case where multiple users can provide input to the same application will register multiple users
		 *
		 * note: the array my contain invalid entries, users have associated indices that they expect to maintain, independent of the existence of other users
		 */
		std::vector<std::shared_ptr<FSlateUser>> m_users;

		//------calculate frame per second------
		DoDoUtf8String calculate_frame_per_second() const;
		//------calculate frame per second------
	};
}
