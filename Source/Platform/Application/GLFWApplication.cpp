#include <PreCompileHeader.h>

#include "GLFWApplication.h"

#include "ApplicationCore/GenericPlatform/GenericApplicationMessageHandler.h"
#include "GLFW/glfw3.h"
#include "Platform/Window/WindowsCursor.h"//FWindowCursor depends on it
#include "Platform/Window/WindowsWindow.h"

namespace DoDo {
	std::shared_ptr<GLFWApplication> window_application = nullptr;//todo:may be raw pointer type

	std::shared_ptr<WindowsWindow> find_window_by_glfw_window(const std::vector<std::shared_ptr<WindowsWindow>> windows_to_search, GLFWwindow* handle_to_find)
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

		int32_t native_window_pos_x, native_window_pos_y;
		glfwGetWindowPos(native_window, &native_window_pos_x, &native_window_pos_y);

		int32_t absolute_cursor_pos_x, absolute_cursor_pos_y;
		absolute_cursor_pos_x = native_window_pos_x + x_pos;
		absolute_cursor_pos_y = native_window_pos_y + y_pos;

		//WM_SETCURSOR
		//todo:add os window border and regular window check
		application->get_message_handler()->On_Cursor_Set();//note:this function will generate a update cursor request

		application->get_message_handler()->On_Mouse_Move(absolute_cursor_pos_x, absolute_cursor_pos_y);//todo:call this function

		//application->get_message_handler()->set_cursor_pos(glm::vec2(x_pos, y_pos));

		//std::cout << "window pos:(" << native_window_pos_x << ", " << native_window_pos_y << ")" << std::endl;
		//
		//std::cout << "mouse pos:(" << x_pos << ", " << y_pos << ")" << std::endl;

		//todo:check window is regular window

		application->get_message_handler()->get_window_zone_for_point(window, x_pos, y_pos);//note:get window zone for point need local mouse pos
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

		int32_t native_window_pos_x, native_window_pos_y;
		glfwGetWindowPos(native_window, &native_window_pos_x, &native_window_pos_y);

		int32_t absolute_cursor_pos_x, absolute_cursor_pos_y;
		absolute_cursor_pos_x = native_window_pos_x + x_pos;
		absolute_cursor_pos_y = native_window_pos_y + y_pos;

		if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
		{
			application->get_message_handler()->set_cursor_pos(glm::vec2(absolute_cursor_pos_x, absolute_cursor_pos_y));//todo:first to update cursor storage position information
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

	static void window_pos_callback(GLFWwindow* native_window, int32_t x_pos, int32_t y_pos)
	{
		GLFWApplication* application = (GLFWApplication*)glfwGetWindowUserPointer(native_window);

		//find window
		const std::shared_ptr<WindowsWindow> window = find_window_by_glfw_window(application->get_native_windows(), native_window);

		//note:noting to do
		//std::cout << x_pos << " " << y_pos << std::endl;
	}

	static void window_close_callback(GLFWwindow* native_window)
	{
		GLFWApplication* application = (GLFWApplication*)glfwGetWindowUserPointer(native_window);

		//find window
		const std::shared_ptr<WindowsWindow> window = find_window_by_glfw_window(application->get_native_windows(), native_window);

		application->remove_window(window);
	}

	GLFWApplication::GLFWApplication()
		: GenericApplication(std::make_shared<FWindowsCursor>())
	{
		//register messenger callback
	}
	GLFWApplication::~GLFWApplication()
	{
		glfwTerminate();
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

		glfwSetWindowPosCallback(native_window_handle, window_pos_callback);

		glfwSetWindowCloseCallback(native_window_handle, window_close_callback);
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

	FPlatformRect GLFWApplication::get_work_area(const FPlatformRect& current_window) const
	{
		return get_work_area_from_os(current_window);
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
	void GLFWApplication::remove_window(std::shared_ptr<Window> window)
	{
		auto it = std::find(m_windows.begin(), m_windows.end(), window);

		if (it != m_windows.end())
		{
			m_windows.erase(it);
		}
	}
	FPlatformRect GLFWApplication::get_work_area_from_os(const FPlatformRect& current_window) const
	{
		int32_t count = 0;
		GLFWmonitor** monitors = glfwGetMonitors(&count);

		int32_t x = 0;
		int32_t y = 0;
		int32_t w = 0;
		int32_t h = 0;

		for (int32_t i = 0; i < count; ++i)
		{
			int32_t xpos, ypos, width, height;

			glfwGetMonitorWorkarea(monitors[i], &xpos, &ypos, &width, &height);

			if (xpos <= current_window.left && xpos + width > current_window.left &&
				ypos <= current_window.top && ypos + height > current_window.bottom)
			{
				x = xpos, y = ypos, w = width, h = height;
				break;
			}
		}

		FPlatformRect work_area;
		work_area.left = x;
		work_area.top = y;
		work_area.right = x + w;
		work_area.bottom = y + h;

		return work_area;
	}
}
