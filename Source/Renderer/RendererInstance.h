#pragma once

#include "Core/Core.h"

namespace DoDo {
	class Window;
	class RendererInstance {
	public:

		virtual ~RendererInstance();

		virtual void Destroy() = 0;

		virtual void draw_frame() = 0;

		virtual void wait_for_idle() = 0;
		
		static Scope<RendererInstance> Create(Window& window);
	};
}