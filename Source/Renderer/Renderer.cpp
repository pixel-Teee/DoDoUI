#include <PreCompileHeader.h>

#include "Renderer.h"

//#include "Platform/VulkanRenderer/VulkanRenderer.h"

#include "Platform/VulkanRenderer/SlateVulkanRenderer.h"

namespace DoDo {
	Renderer::~Renderer()
	{
	}

	void Renderer::destroy()
	{
	}

	std::shared_ptr<Renderer> Renderer::Create()
	{
		return std::make_shared<FSlateVulkanRenderer>();
	}
}
