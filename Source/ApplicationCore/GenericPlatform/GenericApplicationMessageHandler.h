#pragma once

#include "glm/vec2.hpp"

namespace DoDo
{
	class Window;

	namespace EMouseButtons
	{
		enum Type
		{
			Left = 0,
			Middle,
			Right,
			Thumb01,
			Thumb02,

			Invalid
		};
	}

	/*interface that defines how to handle interaction with a user via hardware input and output*/
	class FGenericApplicationMessageHandler
	{
	public:

		virtual ~FGenericApplicationMessageHandler(){}

		virtual glm::vec2 get_cursor_pos() const = 0;

		virtual glm::vec2 get_last_cursor_pos() const = 0;

		virtual bool On_Mouse_Down(const std::shared_ptr<Window>& window, const EMouseButtons::Type button, const glm::vec2 cursor_pos)
		{
			return false;
		}

		virtual bool On_Mouse_Down(const std::shared_ptr<Window>& window, const EMouseButtons::Type button)
		{
			return false;
		}

		virtual void set_cursor_pos(const glm::vec2 mouse_coordinate) //todo:implement this?
		{
			
		}

		virtual bool On_Mouse_Up(const EMouseButtons::Type button)
		{
			return false;
		}

		virtual bool On_Mouse_Up(const EMouseButtons::Type button, const glm::vec2 cursor_pos)
		{
			return false;
		}

		virtual bool On_Mouse_Move()
		{
			return false;
		}
	};
}
