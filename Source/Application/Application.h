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
	private:

		//void CreateInstance();

		Scope<RendererInstance> m_renderer_instance;

		Scope<Window> m_p_window;
	};
}