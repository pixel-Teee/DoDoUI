#pragma once

#include "ApplicationCore/GenericPlatform/GenericApplication.h"

namespace DoDo {
	class WindowsWindow;//forward declare
	class GLFWApplication : public GenericApplication
	{
	public:
		GLFWApplication();

		virtual ~GLFWApplication() override;

		virtual void pump_messages(const float time_delta) override;

		virtual std::shared_ptr<Window> make_window() override;

		virtual void initialize_window(const std::shared_ptr<Window>& window, const std::shared_ptr<FGenericWindowDefinition>& in_definition, const std::shared_ptr<Window>& in_parent, const bool b_show_immediately) override;

		//void initialize(GLFWApplication* const application, const std::shared_ptr<WindowsWindow>& in_parent, const bool b_show_immediately);

		/*processes a single window message*/
		int32_t process_message();

		/*defers a windows message for later processing*/
		void defer_message(std::shared_ptr<WindowsWindow>& native_window, int32_t mouse_x = 0, int32_t mouse_y = 0);

		virtual std::shared_ptr<Window> get_window_under_cursor() override;

		virtual FModifierKeyState get_modifier_keys() const;

		virtual FPlatformRect get_work_area(const FPlatformRect& current_window) const override;

		static std::shared_ptr<GLFWApplication> create_windows_application();

		const std::vector<std::shared_ptr<WindowsWindow>> get_native_windows() const;

		void remove_window(std::shared_ptr<Window> window);

		std::shared_ptr<WindowsWindow> m_current_under_cursor_window;//note:to handle this in the defer message 
	private:

		FPlatformRect get_work_area_from_os(const FPlatformRect& current_window) const;

		std::vector<std::shared_ptr<WindowsWindow>> m_windows;
	};
}