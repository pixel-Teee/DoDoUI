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

	namespace EWindowZone
	{
		/*
		 * the window zone is the window area we are currently over to send back to the operating system
		 * for operating system compliance
		 */
		enum Type
		{
			NotInWindow  = 0,
			TopLeftBorder = 1,
			TopBorder = 2,
			TopRightBorder = 3,
			LeftBorder = 4,
			ClientArea = 5,
			RightBorder = 6,
			BottomLeftBorder = 7,
			BottomBorder = 8,
			BottomRightBorder = 9,
			TitleBar = 10,
			MinimizeButton = 11,
			MaximizeButton = 12,
			CloseButton = 13,
			SysMenu = 14,

			/*no zone specified*/
			Unspecified = 0
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

		virtual bool On_Mouse_Move(double x, double y)
		{
			return false;
		}

		virtual bool On_Size_Changed(const std::shared_ptr<Window>& window, const int32_t width, const int32_t height, bool b_was_minimized = false)
		{
			return false;
		}

		virtual std::shared_ptr<Window> get_hittest_window()
		{
			return nullptr;
		}

		virtual bool On_Cursor_Set()
		{
			return false;
		}
	};
}
