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

		virtual FModifierKeyState get_modifier_keys() const;

		static std::shared_ptr<GLFWApplication> create_windows_application();

		const std::vector<std::shared_ptr<WindowsWindow>> get_native_windows() const;
	private:

		std::vector<std::shared_ptr<WindowsWindow>> m_windows;
	};
}