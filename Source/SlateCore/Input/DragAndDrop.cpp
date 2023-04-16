#include <PreCompileHeader.h>

#include "DragAndDrop.h"

#include "SlateCore/Widgets/SWidget.h"

#include "Application/Application.h"//On_Dragged depends on it

#include "SlateCore/Widgets/SWindow.h"//SWidow depends on it

namespace DoDo {
	FDragDropOperation::FDragDropOperation()
		: m_b_create_new_window(true)
	{
	}
	FDragDropOperation::~FDragDropOperation()
	{
		destroy_cursor_decorator_window();
	}

	void FDragDropOperation::On_Dragged(const FDragDropEvent& drag_drop_event)
	{
		if (m_cursor_decorator_window)
		{
			Application& slate_application = Application::get();

			//todo:implement get cursor size
			glm::vec2 position = drag_drop_event.get_screen_space_position() + Application::get().get_cursor_size();

			if (position != m_cursor_decorator_window->get_position_in_screen()) //note:adjust window position
			{
				FSlateRect anchor(position.x, position.y, position.x, position.y);

				position = slate_application.calculate_tooltip_window_position(anchor, m_cursor_decorator_window->get_desired_size_desktop_pixels(), false);
			}

			//todo:move window
			m_cursor_decorator_window->move_window_to(position);
		}
	}

	void FDragDropOperation::On_Drop(bool b_drop_was_handled, const FPointerEvent& mouse_event)
	{
		destroy_cursor_decorator_window();
	}

	void FDragDropOperation::Construct()
	{
		if (m_b_create_new_window)
		{
			create_cursor_decorator_window();
		}
	}

	void FDragDropOperation::create_cursor_decorator_window()
	{
		std::shared_ptr<SWidget> decorator_to_use = get_default_decorator();

		if (decorator_to_use)
		{
			//todo:implement make cursor decorator
			m_cursor_decorator_window = SWindow::make_cursor_decorator();
			m_cursor_decorator_window->set_content(decorator_to_use);

			Application::get().add_window(m_cursor_decorator_window, true);
		}
	}

	void FDragDropOperation::destroy_cursor_decorator_window()
	{
		if (m_cursor_decorator_window)
		{
			m_cursor_decorator_window->request_destroy_window();
			m_cursor_decorator_window.reset();
		}
	}

}