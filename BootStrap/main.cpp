#include <Application/Application.h>

#include "Core/String/DoDoString.h"

#ifdef Android
#include "Platform/Window/AndroidWindow.h"
#include <game-activity/GameActivity.cpp>
#include <game-text-input/gametextinput.cpp>
extern "C" {
    #include <game-activity/native_app_glue/android_native_app_glue.c>
    void android_main(struct android_app* state);
}

void on_app_cmd(android_app *p_state, int32_t cmd)
{
    switch (cmd)
    {
        case APP_CMD_INIT_WINDOW: {
            DoDo::Application& app = *static_cast<DoDo::Application*>(p_state->userData);
            DoDo::AndroidWindow& android_window = static_cast<DoDo::AndroidWindow&>(app.get_window());
            android_window.set_android_window(p_state->window);
            android_window.set_android_app(p_state);
            app.Init();
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
    DoDo::Application app;
    p_state->userData = &app;
    p_state->onAppCmd = on_app_cmd;
    DoDo::Window& window = app.get_window();

    int events;
    android_poll_source *pSource;
    do {
        //process all pending events before running game logic
        if (ALooper_pollAll(0, nullptr, &events, (void **) &pSource) >= 0) {
            if (pSource) {
                pSource->process(p_state, pSource);
            }
        }

        if(window.get_window_native_handle() != nullptr)
        {
            app.Tick();
        }
    } while (!p_state->destroyRequested);


}
#else
int main()
{

    DoDo::Application app;

    app.Init();

    app.Tick();

    return 0;
}
#endif