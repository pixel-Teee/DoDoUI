#include <PreCompileHeader.h>

#include "WindowsPlatformApplicationMisc.h"

#include "GLFWApplication.h"//specific application

#include "glfw/glfw3.h"

namespace DoDo {
	void FWindowsPlatformApplicationMisc::platform_pre_init()
	{
		//init glfw
		if (!glfwInit())
		{
			std::cout << "create glfw window error!" << std::endl;
			return;
		}
	}
	float FWindowsPlatformApplicationMisc::get_dpi_scale_factor_at_point(float x, float y)
	{
		float scale_x = 1.0f;
		float scale_y = 1.0f;

		//to get the glfw monitor

		GLFWmonitor* monitor;

		int32_t pos_x = static_cast<int32_t>(x);
		int32_t pos_y = static_cast<int32_t>(y);

		int32_t count = 0;
		GLFWmonitor** monitors = glfwGetMonitors(&count);

		GLFWmonitor* find_monitor = nullptr;

		for (int32_t i = 0; i < count; ++i)
		{
			GLFWmonitor* current_monitor = monitors[i];

			//interms position to get the monitor
			const GLFWvidmode* mode = glfwGetVideoMode(current_monitor);

			int32_t left_top_corner_pos_x = 0;
			int32_t left_top_corner_pos_y = 0;

			glfwGetMonitorPos(current_monitor, &left_top_corner_pos_x, &left_top_corner_pos_y);

			int32_t right_down_corner_pos_x = left_top_corner_pos_x + mode->width;
			int32_t right_down_corner_pos_y = left_top_corner_pos_y + mode->height;

			if (pos_x >= left_top_corner_pos_x && pos_x <= right_down_corner_pos_x && pos_y >= left_top_corner_pos_y && pos_y <= right_down_corner_pos_y)
			{
				find_monitor = current_monitor;
				break;
			}
		}

		assert(find_monitor != nullptr);

		//to get dpi scale
		glfwGetMonitorContentScale(find_monitor, &scale_x, &scale_y);

		return scale_x;
	}

	std::shared_ptr<GenericApplication> FWindowsPlatformApplicationMisc::create_application()
	{
		//todo:add app icon

		//dispatch specific os class to create windows application
		return GLFWApplication::create_windows_application();
	}
}
