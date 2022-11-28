#pragma once

#include "Core/Core.h"

namespace DoDo {
	class RendererInstance;
	class Window {
	public:
		virtual void Update(RendererInstance& render_instance) = 0;

		virtual ~Window() {}

		virtual void* get_window_native_handle() = 0;

		static Scope<Window> Create();
	};
}