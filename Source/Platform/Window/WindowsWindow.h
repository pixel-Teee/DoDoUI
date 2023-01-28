#pragma once

#include "Core/Window.h"

struct GLFWwindow;
namespace DoDo {
	class GLFWApplication;
	class WindowsWindow : public Window
	{
	public:
		WindowsWindow();

		virtual ~WindowsWindow();

		virtual void* get_window_native_handle() override;

		virtual void Update(RendererInstance& render_instance) override;

		void initialize(GLFWApplication* const application, const std::shared_ptr<FGenericWindowDefinition>& in_definition, const std::shared_ptr<WindowsWindow>& in_parent, const bool b_show_immediately);
	private:
		//windows window owner this lifetime
		/*the window's handle*/
		GLFWwindow* m_p_window;

		/*the application that owns this window*/
		GLFWApplication* owning_application;

		//static bool m_is_initialized_glfw;
	};
}

