#pragma once

#include "ApplicationCore/GenericPlatform/GenericApplication.h"

namespace DoDo {
	class WindowsWindow;//forward declare

	class GLFWApplication : public GenericApplication
	{
	public:
		GLFWApplication();

		virtual ~GLFWApplication() override;
	private:

		std::vector<std::shared_ptr<WindowsWindow>> m_windows;
	};
}