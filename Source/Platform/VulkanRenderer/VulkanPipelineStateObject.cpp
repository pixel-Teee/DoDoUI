#include <PreCompileHeader.h>

#include "VulkanPipelineStateObject.h"

#include "SlateVulkanRenderer.h"//vertex input layout

#include "Platform/VulkanRenderer/VulkanShader.h"

namespace DoDo {
	const std::vector<VkDynamicState> dynamic_state_enables = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR,
		VK_DYNAMIC_STATE_LINE_WIDTH
	};

	GraphicsPipelineStateObject::GraphicsPipelineStateObject(void* logic_device)
	{
		m_pipeline_info = {};
		m_pipeline_layout_create_info = {};
		m_shader_stage_create_info[0] = m_shader_stage_create_info[1] = {};
		m_pipeline_layout = VK_NULL_HANDLE;
		m_pipeline = VK_NULL_HANDLE;
		m_render_pass = VK_NULL_HANDLE;//todo:remove this
		
	}
	GraphicsPipelineStateObject::~GraphicsPipelineStateObject()
	{

	}
	void GraphicsPipelineStateObject::Destroy(void* logic_device)
	{
		VkDevice device = *(VkDevice*)logic_device;

		//vkDestroyRenderPass(device, m_render_pass, nullptr);

		vkDestroyPipelineLayout(device, m_pipeline_layout, nullptr);

		vkDestroyPipeline(device, m_pipeline, nullptr);
	}
	void GraphicsPipelineStateObject::set_vertex_shader(Shader& vertex_shader, const std::string& target_point)
	{
		//VkShaderModule vertex_shader_module = *(*VkShaderModule)vertex_shader.get_native_handle();

		VkShaderModule vertex_shader_module = *(VkShaderModule*)(vertex_shader.get_native_handle());

		//VkPipelineShaderStageCreateInfo vertex_shader_stage_info{};
		m_shader_stage_create_info[0] = {};
		m_shader_stage_create_info[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		m_shader_stage_create_info[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
		m_shader_stage_create_info[0].module = vertex_shader_module;
		m_shader_stage_create_info[0].pName = target_point.c_str();
	}
	void GraphicsPipelineStateObject::set_pixel_shader(Shader& pixel_shader, const std::string& target_point)
	{
		VkShaderModule pixel_shader_module = *(VkShaderModule*)(pixel_shader.get_native_handle());

		//VkPipelineShaderStageCreateInfo pixel_shader_stage_info{};
		m_shader_stage_create_info[1] = {};
		m_shader_stage_create_info[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		m_shader_stage_create_info[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		m_shader_stage_create_info[1].module = pixel_shader_module;
		m_shader_stage_create_info[1].pName = target_point.c_str();
	}
	void* GraphicsPipelineStateObject::get_render_pass_native_handle()
	{
		return &m_render_pass;
	}

	void* GraphicsPipelineStateObject::get_pipeline_layout()
	{
		return &m_pipeline_layout;
	}

	void* GraphicsPipelineStateObject::get_native_handle()
	{
		return &m_pipeline;
	}

	void GraphicsPipelineStateObject::set_render_pass(void* in_render_pass)
	{
		//render pass cat
		VkRenderPass render_pass = *(VkRenderPass*)in_render_pass;

		m_pipeline_info.renderPass = render_pass;//set render pass
	}

	void GraphicsPipelineStateObject::finalize(void* logic_device)
	{
		VkDevice device = *(VkDevice*)logic_device;

		//------vertex layout------
		//VkPipelineVertexInputStateCreateInfo vertex_input_info{};
		//vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		//vertex_input_info.vertexBindingDescriptionCount = 0;
		//vertex_input_info.pVertexBindingDescriptions = nullptr;//optional
		//vertex_input_info.vertexAttributeDescriptionCount = 0;
		//vertex_input_info.pVertexAttributeDescriptions = nullptr;//optional
		//------vertex layout------

		//------input assembly------
		VkPipelineInputAssemblyStateCreateInfo input_assembly{};
		input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		input_assembly.primitiveRestartEnable = VK_FALSE;
		//------input assembly------

		//------viewport and scissor------
		VkViewport viewport;
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = 1280;
		viewport.height = 720;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		VkExtent2D extent;
		extent.width = 1280;
		extent.height = 720;
		scissor.extent = extent;
		VkPipelineViewportStateCreateInfo viewport_state{};
		viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewport_state.viewportCount = 1;
		viewport_state.pViewports = &viewport;
		viewport_state.scissorCount = 1;
		viewport_state.pScissors = &scissor;
		//------viewport and scissor------

		//------raster------
		VkPipelineRasterizationStateCreateInfo rasterizer{};
		rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizer.depthClampEnable = VK_FALSE;
		rasterizer.rasterizerDiscardEnable = VK_FALSE;
		rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizer.lineWidth = 1.0f;
		rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
		rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
		rasterizer.depthBiasEnable = VK_FALSE;
		rasterizer.depthBiasConstantFactor = 0.0f; // optional
		rasterizer.depthBiasClamp = 0.0f; // optional
		rasterizer.depthBiasSlopeFactor = 0.0f; // optional
		//------raster------

		//------multisampler------
		VkPipelineMultisampleStateCreateInfo multisampling{};
		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.sampleShadingEnable = VK_FALSE;
		multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		multisampling.minSampleShading = 1.0f; // Optional
		multisampling.pSampleMask = nullptr; // Optional
		multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
		multisampling.alphaToOneEnable = VK_FALSE; // Optional
		//------multisampler------

		//------blend------
		VkPipelineColorBlendAttachmentState color_blend_attachment{};
		color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		color_blend_attachment.blendEnable = VK_FALSE;
		color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // optional
		color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // optional
		color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD; // optional
		color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // optional
		color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // optional
		color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD; // optional

		VkPipelineColorBlendStateCreateInfo color_blending{};
		color_blending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		color_blending.logicOpEnable = VK_FALSE;
		color_blending.logicOp = VK_LOGIC_OP_COPY; // optional
		color_blending.attachmentCount = 1;
		color_blending.pAttachments = &color_blend_attachment;
		color_blending.blendConstants[0] = 0.0f; // optional
		color_blending.blendConstants[1] = 0.0f; // optional
		color_blending.blendConstants[2] = 0.0f; // optional
		color_blending.blendConstants[3] = 0.0f; // optional
		//------blend------

		//------depth test------
		VkPipelineDepthStencilStateCreateInfo depth_stencil{};
		depth_stencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depth_stencil.depthTestEnable = VK_TRUE;
		depth_stencil.depthWriteEnable = VK_TRUE;
		depth_stencil.depthCompareOp = VK_COMPARE_OP_LESS;
		depth_stencil.depthBoundsTestEnable = VK_FALSE;
		depth_stencil.minDepthBounds = 0.0f; // optional
		depth_stencil.maxDepthBounds = 1.0f; // optional
		depth_stencil.stencilTestEnable = VK_FALSE;
		depth_stencil.front = {}; // optional
		depth_stencil.back = {}; // optional
		//------depth test------

		//------root signature------
		m_pipeline_layout_create_info = {};
		m_pipeline_layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		m_pipeline_layout_create_info.setLayoutCount = 0;//optional
		m_pipeline_layout_create_info.pSetLayouts = nullptr;//optional
		m_pipeline_layout_create_info.pushConstantRangeCount = 0;//optional
		m_pipeline_layout_create_info.pPushConstantRanges = nullptr;//optional

		//------push constant------
		VkPushConstantRange push_constant;
		push_constant.offset = 0;
		push_constant.size = sizeof(glm::mat4);
		push_constant.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

		m_pipeline_layout_create_info.pPushConstantRanges = &push_constant;
		m_pipeline_layout_create_info.pushConstantRangeCount = 1;
		//------push constant------

		if (vkCreatePipelineLayout(device, &m_pipeline_layout_create_info, nullptr, &m_pipeline_layout) != VK_SUCCESS)
		{
			std::cout << "create pipeline layout error" << std::endl;
		}
		//------root signature------

		//------render pass(describe render target format and depth stencil target format)------
		//VkAttachmentDescription color_attachment{};
		////VkSurfaceFormatKHR format;
		////format.format = VK_FORMAT_B8G8R8_SRGB;
		////format.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
		//color_attachment.format = VK_FORMAT_B8G8R8A8_UNORM;
		//color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
		//
		////load op and store op determine what to do with the data in the attachment before rendering and after rendering
		//color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		//color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		//
		//color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		//color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		//
		//color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		//color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		//
		//VkAttachmentReference color_attachment_ref{};
		//color_attachment_ref.attachment = 0;
		//color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		//
		//VkSubpassDescription subpass{};
		//subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		//subpass.colorAttachmentCount = 1;
		//subpass.pColorAttachments = &color_attachment_ref;
		//subpass.pDepthStencilAttachment = nullptr;
		//
		//VkSubpassDependency dependency{};
		//dependency.srcSubpass = VK_SUBPASS_EXTERNAL;//build in subpass
		//dependency.dstSubpass = 0;
		//dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		//dependency.srcAccessMask = 0;
		//dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		//dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		//
		//VkRenderPassCreateInfo render_pass_info{};
		//render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		//render_pass_info.attachmentCount = 1;
		//render_pass_info.pAttachments = &color_attachment;
		//render_pass_info.subpassCount = 1;
		//render_pass_info.pSubpasses = &subpass;
		//render_pass_info.dependencyCount = 1;
		//render_pass_info.pDependencies = &dependency;
		//
		//if (vkCreateRenderPass(device, &render_pass_info, nullptr, &m_render_pass) != VK_SUCCESS)
		//{
		//	std::cout << "create render pass error!" << std::endl;
		//}
		//------render pass(describe render target format and depth stencil target format)------

		//------dynamic state------
		VkPipelineDynamicStateCreateInfo pipeline_dynamic_state_create_info{};
		pipeline_dynamic_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		pipeline_dynamic_state_create_info.pDynamicStates = dynamic_state_enables.data();
		pipeline_dynamic_state_create_info.dynamicStateCount = static_cast<uint32_t>(dynamic_state_enables.size());
		//------dynamic state------

		//------pso------
		m_pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		m_pipeline_info.stageCount = 2;
		m_pipeline_info.pStages = m_shader_stage_create_info;
		//m_pipeline_info.pVertexInputState = &vertex_input_info;
		m_pipeline_info.pInputAssemblyState = &input_assembly;
		m_pipeline_info.pViewportState = &viewport_state;
		m_pipeline_info.pRasterizationState = &rasterizer;
		m_pipeline_info.pMultisampleState = &multisampling;
		m_pipeline_info.pDepthStencilState = &depth_stencil;
		m_pipeline_info.pColorBlendState = &color_blending;
		m_pipeline_info.pDynamicState = &pipeline_dynamic_state_create_info;

		m_pipeline_info.layout = m_pipeline_layout;

		//reference to render pass and the index of the subpass
		//m_pipeline_info.renderPass = m_render_pass;
		m_pipeline_info.subpass = 0;
		
		m_pipeline_info.basePipelineHandle = VK_NULL_HANDLE; //optional
		m_pipeline_info.basePipelineIndex = -1;
		//------pso------
		
		if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &m_pipeline_info, nullptr, &m_pipeline) != VK_SUCCESS)
		{
			std::cout << "failed to create graphics pipeline!" << std::endl;
		}
	}

	void GraphicsPipelineStateObject::set_input_vertex_layout(void* input_layout)
	{
		VertexInputDescription vertex_layout = *(VertexInputDescription*)input_layout;

		m_vertex_layout = vertex_layout;

		m_vertex_input_state_create_info = {};

		m_vertex_input_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		m_vertex_input_state_create_info.vertexBindingDescriptionCount = m_vertex_layout.bindings.size();
		m_vertex_input_state_create_info.pVertexBindingDescriptions = m_vertex_layout.bindings.data();
		m_vertex_input_state_create_info.vertexAttributeDescriptionCount = m_vertex_layout.attributes.size();
		m_vertex_input_state_create_info.pVertexAttributeDescriptions = m_vertex_layout.attributes.data();

		m_pipeline_info.pVertexInputState = &m_vertex_input_state_create_info;
	}
}
