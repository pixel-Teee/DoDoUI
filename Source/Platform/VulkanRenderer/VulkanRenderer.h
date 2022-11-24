#pragma once

#include <Renderer/Renderer.h>

namespace DoDo {
	class VulkanRenderer : public UIRenderer
	{
	public:
		VulkanRenderer();

		virtual ~VulkanRenderer();
	};
}