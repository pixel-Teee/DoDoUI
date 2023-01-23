#include <PreCompileHeader.h>

#include "Application.h"

#include <Renderer/Renderer.h>

//#include <vulkan/vulkan.h>

#include "Renderer/RendererInstance.h"
#include "Core/Window.h"
#include "SlateCore/Widgets/SWindow.h"

#include "Slate/Widgets/Layout/SBorder.h"//SBorder depends on it

namespace DoDo
{
    struct FDrawWindowArgs
    {
        //todo:implement FSlateDrawBuffer
        //todo:implement FWidgetPath
    };

    Application::Application()
	    : m_last_tick_time(0.0f)
		, m_average_delta_time(1.0f / 30.0f)
    {
        //m_p_vk_instance = CreateScope<VkInstance>();
        m_p_window = Window::Create();

        std::shared_ptr<SBorder> border;
        std::shared_ptr<SBorder> border2;
        SAssignNew(border, SBorder)
            .ContentScale(1.0f)
            .VAlign(VAlign_Fill)
            .HAlign(HAlign_Fill)
            [
                SAssignNew(border2, SBorder)
                .ContentScale(1.0f)
				.VAlign(VAlign_Fill)
				.HAlign(HAlign_Fill)
            ];
    }
        

    Application::~Application()
    {
        //m_renderer_instance = nullptr;
       // std::cout << "error" << std::endl;
        m_renderer_instance->Destroy();
    }

	void Application::Init()
	{
        //reset work directroy
        //std::filesystem::path work_path("../");
        //std::filesystem::current_path(work_path);
        
        //std::cout << std::filesystem::current_path() << std::endl;

        std::cout << "Hello World" << std::endl;



        m_renderer_instance = RendererInstance::Create(*m_p_window);

        //std::unique_ptr<DoDo::UIRenderer> p_Renderer = DoDo::UIRenderer::Create();    
	}

    void Application::Tick()
    {
        //todo:implement TickPlatform
        //TickPlatform is just something to handle message

        //todo:implement TickTime

        //todo:implement TickAndDrawWidgets

        //TODO:in the future, move this window to SWindow
        m_p_window->Update(*m_renderer_instance);
    }

    Window& Application::get_window()
    {
        return *m_p_window;
    }

    void Application::Tick_And_Draw_Widgets(float delta_time)
    {
        //todo:implement pre tick event delegate
        {
            m_pre_tick_event.Invoke(delta_time);//broad cast
        }
        /*
         * update average time between ticks
         * this is used to monitor how responsive the application "feels"
         */
        {
	        /*
	         * scalar percent of new delta time that contributes to running average
	         * use a lower value to add more smoothing to the average frame rate
	         * a value of 1.0 will disable smoothing
	         */
            const float running_average_scale = 0.1f;

            m_average_delta_time = m_average_delta_time * (1.0f - running_average_scale) + get_delta_time() * running_average_scale;

        }

        //draw all windows
        Draw_Windows();

        {
            m_post_tick_event.Invoke(delta_time);//broad cast
        }
    }

    void Application::Draw_Windows()
    {
        Private_Draw_Windows();
    }

    void Application::Private_Draw_Windows(std::shared_ptr<SWindow> draw_only_this_window)
    {
        //todo:implement FWidgetPath

        //prepass the window
        Draw_Pre_Pass(draw_only_this_window);

        //todo:implement FDrawWindowArgs
        FDrawWindowArgs draw_window_args;

        //todo:implement SWindow
        //get the active modal window
        //todo:implement Draw Window And Children

        //todo:draw tooltip windows

        //todo:draw notification windows

        //draw all windows
        //use of an old-style iterator is intentional here, as slate windows
        //array may be mutated by user logic in draw calls, the iterator
        //prevents us from reading off the end and only keeps an index
        //internally
        for (std::vector<std::shared_ptr<SWindow>>::const_iterator itr = m_windows.begin(); itr != m_windows.end(); ++itr)
        {
            std::shared_ptr<SWindow> current_window = *itr;

            //only draw visible windows or in off-screen rendering mode
            //todo:need pass FDrawWindowArgs
            Draw_Window_And_Children(current_window, draw_window_args);
        }

        //todo:implement renderer's draw windows
        //m_renderer_instance
    }

    void Application::Draw_Pre_Pass(std::shared_ptr<SWindow> draw_only_this_window)
    {
        
    }

    void Application::Draw_Window_And_Children(const std::shared_ptr<SWindow>& window_to_draw, struct FDrawWindowArgs& draw_window_args)
    {
        //todo:lack the FDrawWindowArgs parameter

        /*
         * on mac, where child windows can be on screen if their parent is hidden or minimized, we want to always draw child windows
         * on other platforms we set bDrawChildWindows to true only if we draw the current window
         */

        //todo:this is a mac platform macro
        //bool b_Draw_Child_Windows = ;

        //only draw visible windows or in off-screen rendering mode


        //get the draw window args's out draw buffer to add window element list

        //todo:call SWindow's paint window function

        //draw windowless drag drop operations

        //draw software cursors

        //todo:set bDrawChildWindows to true

        //todo:draw the child windows
    }
}
