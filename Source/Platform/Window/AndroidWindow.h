#pragma once

#include "Core/Window.h"

struct ANativeWindow;
struct android_app;
namespace DoDo{
    class AndroidApplication;
    class AndroidWindow : public Window
    {
    public:
        AndroidWindow();

        virtual ~AndroidWindow();

        virtual void* get_window_native_handle() override;

        virtual void Update(RendererInstance& render_instance) override;

        void initialize(AndroidApplication* const application, const std::shared_ptr<FGenericWindowDefinition>& in_definition, const std::shared_ptr<AndroidWindow>& in_parent, const bool b_show_immediately);

        void set_android_app(android_app* p_android_app);

        void set_android_window(ANativeWindow* p_window);
    private:
        ANativeWindow* m_p_window;
        android_app*  m_p_android_app;

        AndroidApplication* owning_application;//platform application
    };
}