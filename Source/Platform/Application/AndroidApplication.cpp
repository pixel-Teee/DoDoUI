#include <PreCompileHeader.h>

#include "AndroidApplication.h"

#include "Platform/Window/AndroidWindow.h"

namespace DoDo{
    AndroidApplication::AndroidApplication() {

    }

    AndroidApplication::~AndroidApplication() {

    }

    std::shared_ptr <Window> AndroidApplication::make_window() {
        return std::make_shared<AndroidWindow>();
    }

    void AndroidApplication::initialize_window(const std::shared_ptr <Window> &in_window,
                                               const std::shared_ptr <FGenericWindowDefinition> &in_definition,
                                               const std::shared_ptr <Window> &in_parent,
                                               const bool b_show_immediately)
    {
        //there is something real to create window
        const std::shared_ptr<AndroidWindow> window = std::static_pointer_cast<AndroidWindow>(in_window);
        const std::shared_ptr<AndroidWindow> parent_window = std::static_pointer_cast<AndroidWindow>(in_parent);//parent window os handle may be null

        m_window = window;

        //create real os window handle
        window->initialize(this, in_definition, parent_window, b_show_immediately);
    }
}
