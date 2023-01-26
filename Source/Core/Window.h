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

	protected:
		std::shared_ptr<FGenericWindowDefinition> m_definition;
	};
}