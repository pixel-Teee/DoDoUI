#include <PreCompileHeader.h>

#include "Application.h"

//------vulkan for glfw------
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
//------vulkan for glfw------

#include <Renderer/Renderer.h>

namespace DoDo
{
    Application::Application()
    {

    }
        

    Application::~Application()
    {

    }

	void Application::Init()
	{
        std::cout << "Hello World" << std::endl;

        //------init glfw------
        GLFWwindow* window;

        if (!glfwInit())
        {
            std::cout << "create glfw window error!" << std::endl;
            return;
        }
        //------init glfw------

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);//don't create opengl context

        window = glfwCreateWindow(800, 600, "window", nullptr, nullptr);

        while (!glfwWindowShouldClose(window))
        {
            glfwPollEvents();

            //draw frame
        }

        std::unique_ptr<DoDo::UIRenderer> p_Renderer = DoDo::UIRenderer::Create();
	}

	
}
