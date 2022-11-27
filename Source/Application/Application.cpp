#include <PreCompileHeader.h>

#include "Application.h"

#include <Renderer/Renderer.h>

//#include <vulkan/vulkan.h>

#include "Renderer/RendererInstance.h"
#include "Core/Window.h"

namespace DoDo
{
    Application::Application()
    {
        //m_p_vk_instance = CreateScope<VkInstance>();
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

        m_p_window = Window::Create();

        m_renderer_instance = RendererInstance::Create(*m_p_window);

        //std::unique_ptr<DoDo::UIRenderer> p_Renderer = DoDo::UIRenderer::Create();    
	}

    void Application::Run()
    {
        //TODO:in the future, move this window to SWindow
        m_p_window->Update(*m_renderer_instance);
    }

    const Window& Application::get_window()
    {
        return *m_p_window;
    }

	
}
