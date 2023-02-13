#pragma once

#include "ApplicationCore/GenericPlatform/GenericApplication.h"

namespace DoDo{

    class AndroidWindow;
    class AndroidApplication : public GenericApplication
    {
    public:
        AndroidApplication();

        virtual ~AndroidApplication() override;

        virtual std::shared_ptr<Window> make_window() override;//android noting todo

        virtual void initialize_window(const std::shared_ptr<Window>& window, const std::shared_ptr<FGenericWindowDefinition>& in_definition, const std::shared_ptr<Window>& in_parent, const bool b_show_immediately) override;

        //void initialize(GLFWApplication* const application, const std::shared_ptr<WindowsWindow>& in_parent, const bool b_show_immediately);
    private:

        //std::vector<std::shared_ptr<WindowsWindow>> m_windows;
        //just a native window
        std::shared_ptr<AndroidWindow> m_window;
    };
}

