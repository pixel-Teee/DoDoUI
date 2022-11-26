#pragma once

#include "Core/Core.h"

namespace DoDo {
	class Window {
	public:
		virtual void Update() = 0;

		virtual ~Window() {}

		static Scope<Window> Create();
	};
}