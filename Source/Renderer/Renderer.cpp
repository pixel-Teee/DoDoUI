#include "Renderer.h"

#include "Platform/VulkanRenderer/VulkanRenderer.h"

namespace DoDo {
	UIRenderer::~UIRenderer()
	{
	}

	std::unique_ptr<UIRenderer> UIRenderer::Create()
	{
		return std::make_unique<VulkanRenderer>();//transfer owner ship
	}
}
