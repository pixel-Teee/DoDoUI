#include <PreCompileHeader.h>

#include "Shader.h"

#include "Platform/VulkanRenderer/VulkanShader.h"

namespace DoDo {
	Shader::~Shader()
	{
	}
	Scope<Shader> Shader::Create(const std::string& file_path, void* logic_device)
	{
		return CreateScope<VulkanShader>(file_path, logic_device);
	}
}