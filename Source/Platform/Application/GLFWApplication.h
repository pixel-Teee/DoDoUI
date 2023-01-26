#pragma once

#include "ApplicationCore/GenericPlatform/GenericApplication.h"

namespace DoDo {
	class WindowsWindow;//forward declare
	class GLFWApplication : public GenericApplication
	{
	public:
		GLFWApplication();

		virtual ~GLFWApplication() override;

		virtual std::shared_ptr<Window> make_window() override;

		virtual void initialize_window(const std::shared_ptr<Window>& window, const std::shared_ptr<FGenericWindowDefinition>& in_definition, const std::shared_ptr<Window>& in_parent, const bool b_show_immediately) override;

		//void initialize(GLFWApplication* const application, const std::shared_ptr<WindowsWindow>& in_parent, const bool b_show_immediately);
	private:

		std::vector<std::shared_ptr<WindowsWindow>> m_windows;
	};
}