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

	static void cursor_position_call_back(GLFWwindow* native_window, double x_pos, double y_pos)
	{
		GLFWApplication* application = (GLFWApplication*)glfwGetWindowUserPointer(native_window);

		//todo:find WindowsWindow, to call defer message
		const std::shared_ptr<WindowsWindow> window = find_window_by_glfw_window(application->get_native_windows(), native_window);

		//application->get_message_handler()->set_hittest_window(window);
		application->m_current_under_cursor_window = window;//note:handle this in the defer message

		//todo:set cursor position
		//application->get_message_handler()->set_cursor_pos(glm::vec2(x_pos, y_pos));//todo:first to update cursor storage position information
		//todo:implement slate user

		application->get_message_handler()->On_Mouse_Move(x_pos, y_pos);//todo:call this function
	}

	static void mouse_button_call_back(GLFWwindow* native_window, int32_t button, int32_t action, int32_t mods)
	{
		GLFWApplication* application = (GLFWApplication*)glfwGetWindowUserPointer(native_window);

		//find window
		const std::shared_ptr<WindowsWindow> window = find_window_by_glfw_window(application->get_native_windows(), native_window);

		//application->get_message_handler()->set_hittest_window(window);
		application->m_current_under_cursor_window = window;

		double x_pos, y_pos;
		glfwGetCursorPos(native_window, &x_pos, &y_pos);

		if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
		{
			application->get_message_handler()->set_cursor_pos(glm::vec2(x_pos, y_pos));//todo:first to update cursor storage position information
			application->get_message_handler()->On_Mouse_Down(window, EMouseButtons::Left);
		}

		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
		{
			application->get_message_handler()->On_Mouse_Up(EMouseButtons::Left);
		}
	}

	static void window_size_callback(GLFWwindow* native_window, int32_t width, int32_t height) //todo:may be need to call set frame buffer size change
	{
		GLFWApplication* application = (GLFWApplication*)glfwGetWindowUserPointer(native_window);

		//find window
		const std::shared_ptr<WindowsWindow> window = find_window_by_glfw_window(application->get_native_windows(), native_window);

		//todo:add minimized and full screen detected

		const bool b_is_full_screen = false;

		//todo:when in fullscreen windows, rendering size should be determined by the application, do not adjust based on WM_SIZE messages
		if(!b_is_full_screen)
		{
			const bool result = application->get_message_handler()->On_Size_Changed(window, width, height, false);
		}
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

		glfwSetMouseButtonCallback(native_window_handle, mouse_button_call_back);//register mouse button call back

		glfwSetFramebufferSizeCallback(native_window_handle, window_size_callback);//register window resize call back
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

	std::shared_ptr<Window> GLFWApplication::get_window_under_cursor()
	{
		return m_current_under_cursor_window;
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

	const std::vector<std::shared_ptr<WindowsWindow>> GLFWApplication::get_native_windows() const
	{
		return m_windows;//todo:may be const?
	}
}
