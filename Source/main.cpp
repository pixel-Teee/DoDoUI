#include <PreCompileHeader.h> //pch

//------vulkan for glfw------
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
//------vulkan for glfw------

#include <Renderer/Renderer.h>

int main()
{
    std::cout << "Hello World" << std::endl;

    //------init glfw------
    GLFWwindow* window;

    if (!glfwInit())
    {
        std::cout << "create glfw window error!" << std::endl;
        return -1;
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

    
    return 0;
}
