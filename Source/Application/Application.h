#pragma once

#include "Core/Core.h"

#include "Core/Delegates/Delegates.h"

#include <vector>
//#include "SlateCore/Widgets/SWindow.h"

namespace DoDo
{
	class RendererInstance;

	class Window;

	class SWindow;

	class GenericApplication;//platform application

	class Renderer;

	class Application//slate application
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
	public:
		//a black hole for android window
		std::shared_ptr<SWindow> get_first_window();

		std::shared_ptr<SWindow> add_window(std::shared_ptr<SWindow> in_slate_window, const bool b_show_immediately = true);

		std::shared_ptr<Window> make_window(std::shared_ptr<SWindow> in_slate_window, const bool b_show_immediately);
	protected:
		//holds the slate renderer used to render this application
		std::shared_ptr<Renderer> m_renderer;

		//holds a pointer to the current slate application
		static std::shared_ptr<Application> s_current_application;

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
	};
}