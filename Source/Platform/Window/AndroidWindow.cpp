#include <PreCompileHeader.h>

#include "AndroidWindow.h"

#include "Renderer/RendererInstance.h"

#include <android_native_app_glue.h>

#include "ApplicationCore/GenericPlatform/GenericWindowDefinition.h"//generic window definition

namespace DoDo {

    AndroidWindow::AndroidWindow()
    {

    }

    AndroidWindow::~AndroidWindow()
    {

    }

    void *AndroidWindow::get_window_native_handle()
    {
        return m_p_window;
    }

    void AndroidWindow::Update(RendererInstance &render_instance)
    {
        //render_instance.draw_frame();
    }

    void AndroidWindow::set_android_app(android_app* p_android_app) {
        m_p_android_app = p_android_app;
    }

    void AndroidWindow::set_android_window(ANativeWindow *p_window) {
        m_p_window = p_window;
    }

    void AndroidWindow::initialize(AndroidApplication *const application,
                                   const std::shared_ptr<FGenericWindowDefinition> &in_definition,
                                   const std::shared_ptr<AndroidWindow> &in_parent,
                                   const bool b_show_immediately)
    {
        //android just one window

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
        //glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);//don't create opengl context

        //todo:implement SWindow construct
        //m_p_window = glfwCreateWindow(window_width, window_height, m_definition->m_title.c_str(), nullptr, nullptr);

        //note:android window just acquire in some place, message call back to acquire
        //android window just one

        //glfwSetWindowPos(m_p_window, window_x, window_y);
        /*
        * to create the window
        */

        //todo:implement drag and drop operation
    }

}