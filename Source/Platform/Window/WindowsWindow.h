#pragma once

#include "Core/Window.h"

struct GLFWwindow;
namespace DoDo {
	class WindowsWindow : public Window
	{
	public:
		WindowsWindow();

		virtual ~WindowsWindow();

		virtual void* get_window_native_handle() override;

		virtual void Update(RendererInstance& render_instance) override;
	private:
		//windows window owner this lifetime
		GLFWwindow* m_p_window;
	};
}

