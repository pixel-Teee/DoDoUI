#pragma once

#include "SlateVulkanVertexBuffer.h"//vertex buffer
#include "SlateVulkanIndexBuffer.h"//index buffer
#include "SlateCore/Rendering/RenderingCommon.h"//vertex array and index array

#include "SlateCore/Rendering/RenderingPolicy.h"

#include "glm/mat4x4.hpp"

#include "include/vk_mem_alloc.h"//vma

namespace DoDo
{
	class FSlateVulkanTextureManager;
	struct DeletionQueue;
	//class FSlateVertexArray;
	//class FSlateIndexArray;
	class FSlateBatchData;
	class FSlateShaderResource;
	class FSlateVulkanRenderingPolicy : public FSlateRenderingPolicy
	{
	public:
		FSlateVulkanRenderingPolicy(std::shared_ptr<FSlateFontServices> in_slate_font_services, VmaAllocator& allocator, std::shared_ptr<FSlateVulkanTextureManager> in_texture_manager);

		~FSlateVulkanRenderingPolicy();

		void clear_vulkan_buffer(VmaAllocator& allocator);

		void reset_offset();

		void build_rendering_buffers(VmaAllocator& allocator, FSlateBatchData& in_batch_data);

		//void upload_mesh(VmaAllocator& allocator, const FSlateVertexArray& vertex_array, const FSlateIndexArray& index_array);

		void draw_elements(VkDevice device, VkCommandBuffer cmd_buffer, VkPipelineLayout pipeline_layout, VkSampler sampler,const glm::mat4x4& view_projection_matrix, int32_t first_batch_index, const std::vector<FSlateRenderBatch>& render_batches, uint32_t total_vertex_offset, uint32_t total_index_offset);
	private:
		//template<typename T>
		//void upload_mesh_internal(VmaAllocator& allocator, const T& array);

		uint32_t m_last_vertex_buffer_offset;

		uint32_t m_last_index_buffer_offset;

		FSlateVulkanVertexBuffer m_vertex_buffer;

		FSlateVulkanIndexBuffer m_index_buffer;

		std::shared_ptr<DeletionQueue> m_deletion_queue;

		//FSlateShaderResource* m_shader_resource;//black hell, todo:fix me

		FSlateShaderResource* m_white_texture;//don't have life time

		std::vector<VkDescriptorSet> m_descriptor_sets;

		std::shared_ptr<FSlateVulkanTextureManager> m_texture_manager;
	};
}
