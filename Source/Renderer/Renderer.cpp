#include <PreCompileHeader.h>

#include "Renderer.h"

#include "Platform/VulkanRenderer/VulkanRenderer.h"

namespace DoDo {
	Renderer::~Renderer()
	{
	}

	std::shared_ptr<Renderer> Renderer::Create()
	{
		return std::make_shared<VulkanRenderer>();
	}
}
