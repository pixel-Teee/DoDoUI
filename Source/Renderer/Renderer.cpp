#include <PreCompileHeader.h>

#include "Renderer.h"

#include "Platform/VulkanRenderer/VulkanRenderer.h"

namespace DoDo {
	UIRenderer::~UIRenderer()
	{
	}

	Scope<UIRenderer> UIRenderer::Create()
	{
		return CreateScope<VulkanRenderer>();
	}
}
