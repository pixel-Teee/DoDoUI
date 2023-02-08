#pragma once

#include "glm/vec2.hpp"

namespace DoDo
{
	/*interface that defines how to handle interaction with a user via hardware input and output*/
	class FGenericApplicationMessageHandler
	{
	public:

		virtual ~FGenericApplicationMessageHandler(){}

		virtual glm::vec2 get_cursor_pos() const = 0;

		virtual glm::vec2 get_last_cursor_pos() const = 0;

		virtual void set_cursor_pos(const glm::vec2 mouse_coordinate)
		{
			
		}

		virtual bool On_Mouse_Move()
		{
			return false;
		}
	};
}
