#pragma once

#include "Core/Core.h"

namespace DoDo {
	struct FGenericWindowDefinition;
	class RendererInstance;
	class Window {
	public:
		virtual void Update(RendererInstance& render_instance);

		virtual ~Window() {}

		virtual void* get_window_native_handle();

		static Scope<Window> Create();

		/*@return true if native window exists underneath the coordinates*/
		virtual bool is_point_in_window(int32_t x, int32_t y) const;

		/*native windows should implement move window to by relocating the platform-specific window to (x, y)*/
		virtual void move_window_to(int32_t x, int32_t y);

		/*native windows should implement this function by asking the os to destroy os-specific resource associated with the window*/
		virtual void destroy();

	protected:
		std::shared_ptr<FGenericWindowDefinition> m_definition;
	};
}