#pragma once

#include <Renderer/Renderer.h>

namespace DoDo {
	class VulkanRenderer : public Renderer
	{
	public:
		VulkanRenderer();

		virtual ~VulkanRenderer();
	};
}