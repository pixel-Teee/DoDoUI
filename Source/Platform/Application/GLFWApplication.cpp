#include <PreCompileHeader.h>

#include "GLFWApplication.h"

#include "Platform/Window/WindowsWindow.h"

namespace DoDo {
	GLFWApplication::GLFWApplication()
	{
		//register messenger callback
	}
	GLFWApplication::~GLFWApplication()
	{
	}
	std::shared_ptr<Window> GLFWApplication::make_window()
	{
		return std::make_shared<WindowsWindow>();//platform related window
	}
	void GLFWApplication::initialize_window(const std::shared_ptr<Window>& in_window, const std::shared_ptr<FGenericWindowDefinition>& in_definition, const std::shared_ptr<Window>& in_parent, const bool b_show_immediately)
	{
		const std::shared_ptr<WindowsWindow> window = std::static_pointer_cast<WindowsWindow>(in_window);
		const std::shared_ptr<WindowsWindow> parent_window = std::static_pointer_cast<WindowsWindow>(in_parent);//parent window os handle may be null

		m_windows.push_back(window);

		//create real os window handle
		window->initialize(this, parent_window, b_show_immediately);
	}
	
}