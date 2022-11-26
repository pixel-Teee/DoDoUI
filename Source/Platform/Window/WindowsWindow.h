#pragma once

#include "Core/Window.h"

struct GLFWwindow;
namespace DoDo {
	class WindowsWindow : public Window
	{
	public:
		WindowsWindow();

		virtual ~WindowsWindow();

		virtual void Update() override;
	private:
		//windows window owner this lifetime
		GLFWwindow* m_p_window;
	};
}

