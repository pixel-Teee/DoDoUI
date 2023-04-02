#include <PreCompileHeader.h>

#include "WindowsWindow.h"

//------vulkan for glfw------
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
//------vulkan for glfw------

#include "Renderer/RendererInstance.h"

#include "ApplicationCore/GenericPlatform/GenericWindowDefinition.h"//generic window definition

#include "glm/glm.hpp"

namespace DoDo {
    //bool WindowsWindow::m_is_initialized_glfw = false;

    //p p1 x p p2
    int64_t get_cross(int64_t p1x, int64_t p1y, int64_t p2x, int64_t p2y, int64_t px, int64_t py)
    {
        return (p1x - px) * (p2y - py) - (p1y - py) * (p2x - px);
    }

	WindowsWindow::WindowsWindow()
	{
        ////------init glfw------
        //m_p_window = nullptr;
        //
        //if (!glfwInit())
        //{
        //    std::cout << "create glfw window error!" << std::endl;
        //    return;
        //}
        ////------init glfw------
        //
        //glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);//don't create opengl context
        //
        //m_p_window = glfwCreateWindow(1280, 720, "window", nullptr, nullptr);
	}
	WindowsWindow::~WindowsWindow()
	{
        if (m_p_window != nullptr)
        {
            glfwDestroyWindow(m_p_window);

            glfwTerminate();

            m_p_window = nullptr;
        }
	}

    void* WindowsWindow::get_window_native_handle()
    {
        return m_p_window;
    }

    void WindowsWindow::Update(RendererInstance& render_instance)
    {
        while (!glfwWindowShouldClose(m_p_window))
        {
            glfwPollEvents();

            //draw frame
            render_instance.draw_frame();
        }

        render_instance.wait_for_idle();
    }

    bool WindowsWindow::is_point_in_window(int32_t x, int32_t y) const
    {
       // bool result = false;

        //int64_t a = get_cross(m_region_width, 0, x, y, 0, 0);
        //int64_t b = get_cross(0, m_region_height, x, y, m_region_width, m_region_height);
        //int64_t c = get_cross(x, y, 0, m_region_height, 0, 0);
        //int64_t d = get_cross(x, y, m_region_width, 0, m_region_width, m_region_height);

        return true;

        //todo:is point in?
        //get_cross(m_region_width, 0, x, y, 0, 0)* get_cross(0, m_region_height, x, y, m_region_width, m_region_height);
    }

    void WindowsWindow::initialize(GLFWApplication* const application, const std::shared_ptr<FGenericWindowDefinition>& in_definition, const std::shared_ptr<WindowsWindow>& in_parent, const bool b_show_immediately)
    {
        m_definition = in_definition;//some window information
        owning_application = application;//don't owns the life time of application

        int32_t window_x = m_definition->x_desired_position_on_screen;
        int32_t window_y = m_definition->y_desired_position_on_screen;
        int32_t window_width = m_definition->m_width_desired_on_screen;//todo:need to trunc to int
        int32_t window_height = m_definition->m_height_desired_on_screen;

		//glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
		glfwWindowHint(GLFW_DECORATED, GL_FALSE);
       
        /*
        * finally, let's initialize the new native window object
        * calling this function will often cause os window messages to be sent(such as messages)
        */
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);//don't create opengl context

        //todo:implement SWindow construct
        m_p_window = glfwCreateWindow(window_width, window_height, m_definition->m_title.c_str(), nullptr, nullptr);

        glfwSetWindowPos(m_p_window, window_x, window_y);
        /*
        * to create the window
        */

        adjust_window_region(window_width, window_height);

        //todo:implement drag and drop operation

        //todo:implement reshape window
        
    }

    void WindowsWindow::adjust_window_region(int32_t width, int32_t height)
    {
        m_region_width = width;
        m_region_height = height;

        //todo:create make window region object
    }

    /*native windows should implement move window to by relocating the client area of the platform-specific window to (x, y)*/
    void WindowsWindow::move_window_to(int32_t x, int32_t y)
    {
        //slate gives the window position as relative to the client area of a window, so we may need to compensate for the os border
        
        //todo:check has os window border

        glfwSetWindowPos(m_p_window, x, y);
    }
    void WindowsWindow::destroy()
    {
        //glfwDestroyWindow(m_p_window);

        glfwSetWindowShouldClose(m_p_window, true);
    }
}