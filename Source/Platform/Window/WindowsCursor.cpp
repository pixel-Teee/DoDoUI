#include <PreCompileHeader.h>

#include "WindowsCursor.h"

#include "GLFW/glfw3.h"

#include "Core/Window.h"//Window depends on it

namespace DoDo
{
	FWindowsCursor::FWindowsCursor()
	{
		//todo:implement this
		//create cursor

		//load up cursors that we'll be using
		for (int32_t cur_cursor_index = 0; cur_cursor_index < EMouseCursor::TotalCursorCount; ++cur_cursor_index)
		{
			m_cursor_handles[cur_cursor_index] = nullptr; //note:set nullptr
			m_cursor_override_handles[cur_cursor_index] = nullptr;

			switch (cur_cursor_index)
			{
			case EMouseCursor::Default:
				m_cursor_handles[EMouseCursor::Default] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
				break;
			case EMouseCursor::TextEditBeam:
				m_cursor_handles[EMouseCursor::TextEditBeam] = glfwCreateStandardCursor(GLFW_IBEAM_CURSOR);
				break;
			case EMouseCursor::Crosshairs:
				m_cursor_handles[EMouseCursor::Crosshairs] = glfwCreateStandardCursor(GLFW_CROSSHAIR_CURSOR);
				break;
			case EMouseCursor::ResizeLeftRight:
				m_cursor_handles[EMouseCursor::ResizeLeftRight] = glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);
				break;
			case EMouseCursor::ResizeUpDown:
				m_cursor_handles[EMouseCursor::ResizeUpDown] = glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR);
				break;
			case EMouseCursor::Hand:
				m_cursor_handles[EMouseCursor::Hand] = glfwCreateStandardCursor(GLFW_HAND_CURSOR);
				break;
			}
		}

		//set the default cursor
		set_type(EMouseCursor::Default);
	}

	FWindowsCursor::~FWindowsCursor()
	{
		//glfwSetCursorPos()

		for (int32_t cur_cursor_index = 0; cur_cursor_index < EMouseCursor::TotalCursorCount; ++cur_cursor_index)
		{
			if (m_cursor_handles[cur_cursor_index] != nullptr)
			{
				glfwDestroyCursor(m_cursor_handles[cur_cursor_index]);
			}

			if (m_cursor_override_handles[cur_cursor_index] != nullptr)
			{
				glfwDestroyCursor(m_cursor_override_handles[cur_cursor_index]);
			}
		}
	}

	void FWindowsCursor::get_size(int32_t& x, int32_t& y) const
	{
		//todo:this is wrong
		x = 16;
		y = 16;
	}

	void FWindowsCursor::set_position(const int32_t x, const int32_t y)
	{
		//todo:implement this function
	}

	void FWindowsCursor::set_type(const EMouseCursor::Type in_new_cursor)
	{
		m_current_type = in_new_cursor;
		//todo:set cursor
	}

	void FWindowsCursor::set_type(const std::shared_ptr<Window> window, const EMouseCursor::Type in_new_cursor)
	{
		m_current_type = in_new_cursor;

		glfwSetCursor((GLFWwindow*)(window->get_window_native_handle()), m_cursor_handles[in_new_cursor]);
	}

	EMouseCursor::Type FWindowsCursor::get_type() const
	{
		return m_current_type;
	}

	glm::vec2 FWindowsCursor::get_position() const
	{
		//from cursor to get position
		//return glfwGetCursorPos();
		//todo:need pass window
		return glm::vec2(0.0f);
	}
}
