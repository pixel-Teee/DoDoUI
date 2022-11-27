#include "Renderer/PipelineStateObject.h"

#include <vulkan/vulkan.h>

namespace DoDo {
	class GraphicsPipelineStateObject : public PipelineStateObject
	{
	public:
		GraphicsPipelineStateObject(void* logic_device);

		virtual ~GraphicsPipelineStateObject();

		virtual void Destroy(void* logic_device) override;

		virtual void set_vertex_shader(Shader& vertex_shader, const std::string& target_point) override;

		virtual void set_pixel_shader(Shader& pixel_shader, const std::string& target_point) override;
		
		virtual void* get_render_pass_native_handle() override;

		virtual void* get_native_handle() override;

		virtual void finalize(void* logic_device) override;
	private:
		//------root signature------
		VkPipelineLayoutCreateInfo m_pipeline_layout_create_info;

		VkPipelineLayout m_pipeline_layout;
		//------root signature------

		//------pipeline------
		VkPipeline m_pipeline;

		VkGraphicsPipelineCreateInfo m_pipeline_info;
		//------pipeline------

		VkPipelineShaderStageCreateInfo m_shader_stage_create_info[2];

		VkRenderPass m_render_pass;
	};
}