#include <PreCompileHeader.h>

#include "WindowsCursor.h"

#include "GLFW/glfw3.h"

namespace DoDo
{
	FWindowsCursor::FWindowsCursor()
	{
		//todo:implement this
		//create cursor
	}

	FWindowsCursor::~FWindowsCursor()
	{
		//glfwSetCursorPos()
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

	glm::vec2 FWindowsCursor::get_position() const
	{
		//from cursor to get position
		return glm::vec2(0.0f, 0.0f);
	}
}
