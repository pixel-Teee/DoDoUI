#include <PreCompileHeader.h>

#include "WindowsWindow.h"

//------vulkan for glfw------
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
//------vulkan for glfw------

#include "Renderer/RendererInstance.h"

#include "ApplicationCore/GenericPlatform/GenericWindowDefinition.h"//generic window definition

namespace DoDo {
    bool WindowsWindow::m_is_initialized_glfw = false;

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
        if (!m_is_initialized_glfw)
        {
            if (!glfwInit())
            {
                std::cout << "create glfw window error!" << std::endl;
                return;
            }
            else
            {
                m_is_initialized_glfw = true;
            }
        }
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
    void WindowsWindow::initialize(GLFWApplication* const application, const std::shared_ptr<FGenericWindowDefinition>& in_definition, const std::shared_ptr<WindowsWindow>& in_parent, const bool b_show_immediately)
    {
        m_definition = in_definition;//some window information
        owning_application = application;//don't owns the life time of application

        int32_t window_x = m_definition->x_desired_position_on_screen;
        int32_t window_y = m_definition->y_desired_position_on_screen;
        int32_t window_width = m_definition->m_width_desired_on_screen;//todo:need to trunc to int
        int32_t window_height = m_definition->m_height_desired_on_screen;

        /*
        * finally, let's initialize the new native window object
        * calling this function will often cause os window messages to be sent(such as messages)
        */
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);//don't create opengl context

        m_p_window = glfwCreateWindow(window_width, window_height, m_definition->m_title.c_str(), nullptr, nullptr);

        glfwSetWindowPos(m_p_window, window_x, window_y);
        /*
        * to create the window
        */

        //todo:implement drag and drop operation

    }
}