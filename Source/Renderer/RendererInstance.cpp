#include <PreCompileHeader.h>

#include "RendererInstance.h"

#include "Platform/VulkanRenderer/VulkanInstance.h"

namespace DoDo {

    RendererInstance::~RendererInstance()
    {

    }

    Scope<RendererInstance> RendererInstance::Create()
    {
        return CreateScope<VulkanInstance>();
    }
}