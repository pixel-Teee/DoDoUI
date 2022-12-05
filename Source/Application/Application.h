#pragma once

#include "Core/Core.h"

namespace DoDo
{
	class RendererInstance;

	class Window;

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
	};
}