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
	class Application
	{
	public:
		Application();

		~Application();

		void Init();

		//void Run();
		void Tick();

		//Scope<Window> get_window();

		Window& get_window();

		/*event before slate application ticks*/
		using FSlateTickEvent = Delegate_Event<void(float)>;
		FSlateTickEvent& on_pre_tick() { return m_pre_tick_event; }

		/*event after slate application ticks*/
		FSlateTickEvent& on_post_tick() { return m_post_tick_event; }

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