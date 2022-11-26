#pragma once

#include "Core/Core.h"

namespace DoDo {
	class Window {
	public:
		virtual void Update() = 0;

		virtual ~Window() {}

		virtual void* get_window_native_handle() = 0;

		static Scope<Window> Create();
	};
}