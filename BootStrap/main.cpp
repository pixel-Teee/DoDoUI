#include <Application/Application.h>

#include <Renderer/Renderer.h>//renderer

#include "Core/String/DoDoString.h"

#include <fstream>

#ifdef WIN32
#include "Platform/Application/WindowsPlatformApplicationMisc.h"
#endif

#ifdef Android

#include "SlateCore/Widgets/SWindow.h"

#include "Platform/Application/AndroidPlatformApplicationMisc.h"

#include "Platform/Window/AndroidWindow.h"
#include <game-activity/GameActivity.cpp>
#include <game-text-input/gametextinput.cpp>
extern "C" {
    #include <game-activity/native_app_glue/android_native_app_glue.c>
    void android_main(struct android_app* state);
}

static bool is_initialized = false;

void on_app_cmd(android_app *p_state, int32_t cmd)
{
    switch (cmd)
    {
        case APP_CMD_INIT_WINDOW: {
            DoDo::Application& app = *static_cast<DoDo::Application*>(p_state->userData);
            //DoDo::AndroidWindow& android_window = static_cast<DoDo::AndroidWindow&>(app.get_window());
            //android_window.set_android_app(p_state);
            //android_window.set_android_window(p_state->window);
            //app.Init();

            //todo:in there to create window and real os window
            std::shared_ptr<DoDo::SWindow> window = DoDo::Application::get().test_create_widget();

            //std::shared_ptr<DoDo::SWindow> first_window = DoDo::Application::get().get_first_window();

            //todo:get SWindow native window
            std::shared_ptr<DoDo::Window> native_window = window->get_native_window();

            std::static_pointer_cast<DoDo::AndroidWindow>(native_window)->set_android_window(p_state->window);
            std::static_pointer_cast<DoDo::AndroidWindow>(native_window)->set_android_app(p_state);

            std::shared_ptr<DoDo::Renderer> pRenderer = DoDo::Renderer::Create();

            DoDo::Application::get().Initialize_Renderer(pRenderer);//initialize renderer

            //note:create surface need renderer initialize
            bool result = DoDo::Application::get().make_platform_window_and_create_view_port(window);
            is_initialized = true;
            break;
        }
        case APP_CMD_CONTENT_RECT_CHANGED: {

            break;
        }
        case APP_CMD_GAINED_FOCUS: {

            break;
        }
        case APP_CMD_LOST_FOCUS: {

            break;
        }
    }
}

void android_main(android_app* p_state)
{
    DoDo::FPlatformApplicationMisc::platform_pre_init();

    DoDo::Application::Create();//initialize platform application

    DoDo::Application app;
    p_state->userData = &app;
    p_state->onAppCmd = on_app_cmd;
    //DoDo::Window& window = app.get_window();

    int events;
    android_poll_source *pSource;
    do {
        //process all pending events before running game logic
        if (ALooper_pollAll(0, nullptr, &events, (void **) &pSource) >= 0) {
            if (pSource) {
                pSource->process(p_state, pSource);
            }
        }

        if(is_initialized)
        {
            DoDo::Application::get().Tick();
        }

        //DoDo::Application::get().Tick();

        //if(window.get_window_native_handle() != nullptr)
        //{
        //    app.Tick();
        //}
    } while (!p_state->destroyRequested);

    DoDo::Application::shut_down();
}
#else

int main()
{
#if 0
    DoDo::Application app;

    app.Init();

    app.Tick();
#else
    //DoDo::DoDoUtf8String default_engine_dir("../../../Content/");
    //DoDo::DoDoUtf8String dir_to_try = default_engine_dir / default_engine_dir;

    DoDo::FPlatformApplicationMisc::platform_pre_init();//todo:need to put at the app init function

    DoDo::Application::Create();//initialize platform application

    std::shared_ptr<DoDo::Renderer> pRenderer = DoDo::Renderer::Create();

    DoDo::Application::get().Initialize_Renderer(pRenderer);//initialize renderer

    DoDo::Application::get().test_create_widget();

    //tick
    //todo:add exit global variable
	//while (true)
	//{
		DoDo::Application::get().Tick();
	//}

    DoDo::Application::shut_down();//todo:in the future, put this at exit function

    //DoDo::Application::get().Tick();
#endif
    return 0;
}
#endif