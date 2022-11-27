#include <PreCompileHeader.h>

#include "PipelineStateObject.h"

#include "Platform/VulkanRenderer/VulkanPipelineStateObject.h"

namespace DoDo {
	PipelineStateObject::~PipelineStateObject()
	{
	}
	Scope<PipelineStateObject> PipelineStateObject::Create(void* logic_device)
	{
		return CreateScope<GraphicsPipelineStateObject>(logic_device);
	}
}