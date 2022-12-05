#include <PreCompileHeader.h>

#include "AndroidWindow.h"

#include "Renderer/RendererInstance.h"

#include <android_native_app_glue.h>

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
        render_instance.draw_frame();
    }

    void AndroidWindow::set_android_app(android_app* p_android_app) {
        m_p_android_app = p_android_app;
    }

    void AndroidWindow::set_android_window(ANativeWindow *p_window) {
        m_p_window = p_window;
    }

}