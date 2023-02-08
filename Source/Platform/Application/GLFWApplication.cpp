#include <PreCompileHeader.h>

#include "GLFWApplication.h"

#include "ApplicationCore/GenericPlatform/GenericApplicationMessageHandler.h"
#include "GLFW/glfw3.h"
#include "Platform/Window/WindowsCursor.h"//FWindowCursor depends on it
#include "Platform/Window/WindowsWindow.h"

namespace DoDo {
	std::shared_ptr<GLFWApplication> window_application = nullptr;//todo:may be raw pointer type

	static std::shared_ptr<WindowsWindow> find_window_by_glfw_window(const std::vector<std::shared_ptr<WindowsWindow>> windows_to_search, GLFWwindow* handle_to_find)
	{
		for(int32_t window_index = 0; window_index < windows_to_search.size(); ++window_index)
		{
			std::shared_ptr<WindowsWindow> window = windows_to_search[window_index];

			if(window->get_window_native_handle() == handle_to_find)
			{
				return window;
			}
		}

		return std::shared_ptr<WindowsWindow>();
	}

	static void cursor_position_call_back(GLFWwindow* window, double x_pos, double y_pos)
	{
		GLFWApplication* application = (GLFWApplication*)glfwGetWindowUserPointer(window);

		//todo:find WindowsWindow, to call defer message

		//todo:set cursor position
		application->get_message_handler()->set_cursor_pos(glm::vec2(x_pos, y_pos));//todo:first to update cursor storage position information
		//todo:implement slate user

		application->get_message_handler()->On_Mouse_Move();//todo:call this function
	}

	GLFWApplication::GLFWApplication()
		: GenericApplication(std::make_shared<FWindowsCursor>())
	{
		//register messenger callback
	}
	GLFWApplication::~GLFWApplication()
	{
	}

	void GLFWApplication::pump_messages(const float time_delta)
	{
		glfwPollEvents();//call message call back
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
		window->initialize(this, in_definition, parent_window, b_show_immediately);

		GLFWwindow* native_window_handle = (GLFWwindow*)(window->get_window_native_handle());
		glfwSetWindowUserPointer(native_window_handle, window_application.get());

		glfwSetCursorPosCallback(native_window_handle, cursor_position_call_back);//register message call back
	}

	int32_t GLFWApplication::process_message()
	{
		//todo:interms native window handle to find WindowsWindow class, then
		return 0;
	}

	//let os native message call back to call this function
	void GLFWApplication::defer_message(std::shared_ptr<WindowsWindow>& native_window, int32_t mouse_x, int32_t mouse_y)
	{

	}

	FModifierKeyState GLFWApplication::get_modifier_keys() const
	{
		return FModifierKeyState(
			false,
			false,
			false,
			false,
			false,
			false,
			false,
			false,
			false
		);//todo:fix me
	}

	std::shared_ptr<GLFWApplication> GLFWApplication::create_windows_application()
	{
		window_application = std::make_shared<GLFWApplication>();
		return window_application;
	}
}
