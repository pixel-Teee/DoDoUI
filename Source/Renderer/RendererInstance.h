#pragma once

#include "Core/Core.h"

namespace DoDo {
	class RendererInstance {
	public:

		virtual ~RendererInstance();
		
		static Scope<RendererInstance> Create();
	};
}