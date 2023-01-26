#include <PreCompileHeader.h>

#include <chrono>

#include "Application.h"

#include <Renderer/Renderer.h>

//#include <vulkan/vulkan.h>

#include "Renderer/RendererInstance.h"
#include "Core/Window.h"
#include "SlateCore/Widgets/SWindow.h"

#include "Slate/Widgets/Layout/SBorder.h"//SBorder depends on it

#include "SlateCore/Rendering/SlateDrawBuffer.h"//FDrawWindowArgs depends on it

#include "Platform/Application/GLFWApplication.h"//GenericApplication depends on it

namespace DoDo
{
    static std::shared_ptr<GenericApplication> s_platform_application = nullptr;//global platform application

    static std::shared_ptr<Application> s_current_application = nullptr;//global slate application

    struct FDrawWindowArgs
    {
        //todo:implement FSlateDrawBuffer
        //todo:implement FWidgetPath
        FDrawWindowArgs(FSlateDrawBuffer& in_draw_buffer)
            : m_out_draw_buffer(in_draw_buffer)
        {}

        FSlateDrawBuffer& m_out_draw_buffer;
    };

    Application::Application()
	    : m_last_tick_time(0.0f)
        , m_current_time(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count())
		, m_average_delta_time(1.0f / 30.0f)
    {
        //m_p_vk_instance = CreateScope<VkInstance>();
        m_p_window = Window::Create();

        //std::shared_ptr<SBorder> border;
        //std::shared_ptr<SBorder> border2;
        //SAssignNew(border, SBorder)
        //    .ContentScale(1.0f)
        //    .VAlign(VAlign_Fill)
        //    .HAlign(HAlign_Fill)
        //    [
        //        SAssignNew(border2, SBorder)
        //        .ContentScale(1.0f)
		//		.VAlign(VAlign_Fill)
		//		.HAlign(HAlign_Fill)
        //    ];
    }
        

    Application::~Application()
    {
        //m_renderer_instance = nullptr;
       // std::cout << "error" << std::endl;
        m_renderer_instance->Destroy();
    }

    void Application::Create()
    {
        Create(GenericApplication::Create());
    }

    std::shared_ptr<Application> Application::Create(const std::shared_ptr<GenericApplication>& in_platform_application)
    {
        //return std::shared_ptr<Application>();
        s_platform_application = in_platform_application;//platform application

        s_current_application = std::make_shared<Application>();//slate application

        return s_current_application;
    }

	void Application::Initialize_Renderer(std::shared_ptr<Renderer> in_renderer)
	{
        //reset work directroy
        //std::filesystem::path work_path("../");
        //std::filesystem::current_path(work_path);
        
        //std::cout << std::filesystem::current_path() << std::endl;

        //std::cout << "Hello World" << std::endl;

        m_renderer = in_renderer;

        //m_renderer->
        //todo:call renderer initialize

        m_renderer_instance = RendererInstance::Create(*m_p_window);

        //std::unique_ptr<DoDo::UIRenderer> p_Renderer = DoDo::UIRenderer::Create();    
	}

    void Application::Tick()
    {
        const float delta_time = get_delta_time();
        //todo:implement TickPlatform
        //TickPlatform is just something to handle message
        tick_platform(delta_time);

        //todo:implement TickTime
        tick_time();

        //todo:implement TickAndDrawWidgets
        Tick_And_Draw_Widgets(delta_time);

        //TODO:in the future, move this window to SWindow
        m_p_window->Update(*m_renderer_instance);
    }

    Window& Application::get_window()
    {
        return *m_p_window;
    }

    void Application::tick_time()
    {
        m_last_tick_time = m_current_time;
        //m_current_time =
        //todo:implement FPlatformTime
        m_current_time = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    }

    void Application::tick_platform(float delta_time)
    {
        //todo:handle messenger
        s_platform_application->Tick(delta_time);
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
    std::shared_ptr<SWindow> Application::add_window(std::shared_ptr<SWindow> in_slate_window, const bool b_show_immediately)
    {
        /*
        * add the slate window to the slate application's top-level window array, note that neither the slate window
        * or the active window are ready to br used yet, however we need to make sure they're in the slate window
        * array so that we can properly respond to OS window messages as soon as they're sent, for example, a window
        * activation message may be sent by the OS as soon as the window is shown(in the init function), and if we
        * don't add the slate window to our window list, we wouldn't be able to route that message to the window
        */

        
    }
    std::shared_ptr<Window> Application::make_window(std::shared_ptr<SWindow> in_slate_window, const bool b_show_immediately)
    {
        std::shared_ptr<Window> native_parent = nullptr;
        std::shared_ptr<SWindow> parent_window = in_slate_window->get_parent_window();

        if (parent_window) //check valid
        {
            native_parent = parent_window->get_native_window();//get parent SWindow's native window
        }

        //get the SWindow's attribute to initialize native window's initialize information
        std::shared_ptr<FGenericWindowDefinition> definition = std::make_shared<FGenericWindowDefinition>();

        definition->m_type = in_slate_window->get_type();

        const glm::vec2 size = in_slate_window->get_initial_desired_size_in_screen();
        definition->m_width_desired_on_screen = size.x;
        definition->m_height_desired_on_screen = size.y;

        const glm::vec2 position = in_slate_window->get_initial_desired_position_in_screen();
        definition->x_desired_position_on_screen = position.x;
        definition->y_desired_position_on_screen = position.y;

        //todo:implement platform application's make window
        std::shared_ptr<Window> new_window = s_platform_application->make_window();

        in_slate_window->set_native_window(new_window);

        s_platform_application->initialize_window(new_window)

        return new_window;
    }
}