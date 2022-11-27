#include <PreCompileHeader.h>

#include "VulkanShader.h"

namespace DoDo {
	static std::vector<char> read_file(const std::string& file_path)
	{
		//std::cout << std::filesystem::current_path() << std::endl;

		//ate : to read at end of the file
		std::ifstream file(file_path, std::ios::ate | std::ios::binary);

		//get the file size
		size_t file_size = (size_t)file.tellg();
		std::vector<char> buffer(file_size);

		file.seekg(0);
		file.read(buffer.data(), file_size);

		if (!file.is_open()) {
			std::cout << "failed to open file!" << std::endl;
		}

		return buffer;
	}

	VulkanShader::VulkanShader()
	{
		m_shader_module = VK_NULL_HANDLE;
	}
	VulkanShader::VulkanShader(const std::string& file_path, void* logic_device)
	{
		std::vector<char> shader_code = read_file(file_path);

		VkShaderModuleCreateInfo create_info{};

		create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		create_info.codeSize = shader_code.size();
		create_info.pCode = reinterpret_cast<const uint32_t*>(shader_code.data());

		VkDevice device = *(VkDevice*)logic_device;

		if (vkCreateShaderModule(device, &create_info, nullptr, &m_shader_module))
		{
			std::cout << "failed to create shader module!" << std::endl;
		}
	}
	void* VulkanShader::get_native_handle()
	{
		return &m_shader_module;
	}
	VulkanShader::~VulkanShader()
	{

	}
	void VulkanShader::Destroy(void* logic_device)
	{
		VkDevice device = *(VkDevice*)logic_device;
		vkDestroyShaderModule(device, m_shader_module, nullptr);
	}
}