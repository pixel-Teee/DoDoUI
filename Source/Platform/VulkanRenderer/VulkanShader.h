#pragma once

#include "Renderer/Shader.h"

#include <vulkan/vulkan.h>

namespace DoDo {
	class VulkanShader : public Shader
	{
	public:
		VulkanShader();

		VulkanShader(const std::string& file_path, void* logic_device);

		virtual void* get_native_handle() override;

		virtual ~VulkanShader();

		virtual void Destroy(void* logic_device) override;
	private:

		VkShaderModule m_shader_module;
	};
}