#pragma once

#include "ApplicationCore/GenericPlatform/ICursor.h"

class GLFWcursor;
namespace DoDo
{
	class Window;
	class FWindowsCursor : public ICursor
	{
	public:
		FWindowsCursor();

		virtual ~FWindowsCursor();

		virtual void set_position(const int32_t x, const int32_t y) override;

		virtual void set_type(const EMouseCursor::Type in_new_cursor) override;

		virtual void set_type(const std::shared_ptr<Window> window, const EMouseCursor::Type in_new_cursor) override;

		glm::vec2 get_position() const override;

	private:
		EMouseCursor::Type m_current_type;

		//GLFWcursor* m_cursor;
		GLFWcursor* m_cursor_override_handles[EMouseCursor::TotalCursorCount];

		/*cursors*/
		GLFWcursor* m_cursor_handles[EMouseCursor::TotalCursorCount];//todo:implement EMouseCursor
	};
}
