#include <PreCompileHeader.h>

#include "WindowsWindow.h"

//------vulkan for glfw------
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
//------vulkan for glfw------

#include "Renderer/RendererInstance.h"

namespace DoDo {
	WindowsWindow::WindowsWindow()
	{
        //------init glfw------
        m_p_window = nullptr;

        if (!glfwInit())
        {
            std::cout << "create glfw window error!" << std::endl;
            return;
        }
        //------init glfw------

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);//don't create opengl context

        m_p_window = glfwCreateWindow(800, 600, "window", nullptr, nullptr);
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
}