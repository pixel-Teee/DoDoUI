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

		virtual bool is_point_in_window(int32_t x, int32_t y) const override;

		void initialize(GLFWApplication* const application, const std::shared_ptr<FGenericWindowDefinition>& in_definition, const std::shared_ptr<WindowsWindow>& in_parent, const bool b_show_immediately);

		/*
		 * sets the window region to specified dimensions
		 *
		 * @param width the width of the window region(in pixels)
		 * @param height the height of the window region(in pixels)
		 */
		void adjust_window_region(int32_t width, int32_t height);

		virtual void move_window_to(int32_t x, int32_t y) override;

		virtual void reshape_window(int32_t new_x, int32_t new_y, int32_t new_width, int32_t new_height);

		virtual void set_opacity(const float in_opacity) override;

		virtual void destroy() override;
	private:
		//windows window owner this lifetime
		/*the window's handle*/
		GLFWwindow* m_p_window;

		/*the application that owns this window*/
		GLFWApplication* owning_application;

		/*stores the window region size for querying whether a point lies within the window*/
		int32_t m_region_width;
		int32_t m_region_height;

		//static bool m_is_initialized_glfw;
	};
}

