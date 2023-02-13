#pragma once

#include "ApplicationCore/GenericPlatform/ICursor.h"

namespace DoDo
{
	class GLFWcursor;
	class FWindowsCursor : public ICursor
	{
	public:
		FWindowsCursor();

		virtual ~FWindowsCursor();

		virtual void set_position(const int32_t x, const int32_t y) override;

		virtual void set_type(const EMouseCursor::Type in_new_cursor) override;

		glm::vec2 get_position() const override;

	private:
		EMouseCursor::Type m_current_type;

		//GLFWcursor* m_cursor;

		/*cursors*/
		std::vector<GLFWcursor*> m_cursors[15];//todo:implement EMouseCursor
	};
}
