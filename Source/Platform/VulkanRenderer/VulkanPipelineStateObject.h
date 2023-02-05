
#include <vulkan/vulkan.h>

#include "SlateVulkanRenderer.h"

#include "Renderer/PipelineStateObject.h"//todo:remove this

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

		virtual void* get_pipeline_layout() override;

		virtual void* get_native_handle() override;

		virtual void set_render_pass(void* render_pass) override;

		virtual void finalize(void* logic_device) override;

		virtual void set_descriptor_set(uint32_t set_counts, void* descriptor_set) override;

		virtual void set_input_vertex_layout(void* input_layout) override;
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

		VertexInputDescription m_vertex_layout;
		VkPipelineVertexInputStateCreateInfo m_vertex_input_state_create_info;

		VkRenderPass m_render_pass;//todo:remove this
	};
}
