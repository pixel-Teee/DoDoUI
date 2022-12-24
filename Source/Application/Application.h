#pragma once

#include "Core/Core.h"

#include <vector>
//#include "SlateCore/Widgets/SWindow.h"

namespace DoDo
{
	class RendererInstance;

	class Window;

	class SWindow;
	class Application
	{
	public:
		Application();

		~Application();

		void Init();

		void Run();	

		//Scope<Window> get_window();

		Window& get_window();
	private:

		//void CreateInstance();

		Scope<RendererInstance> m_renderer_instance;

		Scope<Window> m_p_window;

		//all top-level windows owned by this application, there are tracked here in a platform-agnostic way
		std::vector<Scope<SWindow>> m_windows;
	};
}