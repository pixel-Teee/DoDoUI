#include "PreCompileHeader.h"

#include "Menu.h"

namespace DoDo {
    FMenuBase::FMenuBase(std::shared_ptr<SWidget> in_content, const bool b_collapsed_by_parent)
        : m_content(in_content)
        , m_b_is_collapsed_by_parent(b_collapsed_by_parent)
    {
    }
    FMenuInWindow::FMenuInWindow(std::shared_ptr<SWindow> in_window, std::shared_ptr<SWidget> in_content, const bool b_is_collapsed_by_parent)
        : FMenuBase(in_content, b_is_collapsed_by_parent)
        , m_window(in_window)
    {
    }
    FMenuInWindow::~FMenuInWindow()
    {
    }
    std::shared_ptr<SWindow> FMenuInWindow::get_parent_window() const
    {
        //return the menu's window
        return m_window.lock();
    }
}