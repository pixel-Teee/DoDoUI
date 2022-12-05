#pragma once

#include "Core/Window.h"

struct ANativeWindow;
struct android_app;
namespace DoDo{
    class AndroidWindow : public Window
    {
    public:
        AndroidWindow();

        virtual ~AndroidWindow();

        virtual void* get_window_native_handle() override;

        virtual void Update(RendererInstance& render_instance) override;

        void set_android_app(android_app* p_android_app);

        void set_android_window(ANativeWindow* p_window);
    private:
        ANativeWindow* m_p_window;
        android_app*  m_p_android_app;
    };
}