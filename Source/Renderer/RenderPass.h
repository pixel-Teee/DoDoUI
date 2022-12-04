#pragma once

#include <Core/Core.h>

namespace DoDo {
	class RenderPass
	{
	public:
		virtual ~RenderPass();

		static Scope<RenderPass> Create();
	};
}