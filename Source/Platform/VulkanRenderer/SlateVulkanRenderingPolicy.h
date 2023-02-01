#pragma once

#include "SlateVulkanVertexBuffer.h"//vertex buffer
#include "SlateVulkanIndexBuffer.h"//index buffer
#include "SlateCore/Rendering/RenderingCommon.h"//vertex array and index array

#include "SlateCore/Rendering/RenderingPolicy.h"

#include "glm/mat4x4.hpp"

#include "include/vk_mem_alloc.h"//vma

namespace DoDo
{
	struct DeletionQueue;
	//class FSlateVertexArray;
	//class FSlateIndexArray;
	class FSlateBatchData;
	
	class FSlateVulkanRenderingPolicy : public FSlateRenderingPolicy
	{
	public:
		FSlateVulkanRenderingPolicy();

		~FSlateVulkanRenderingPolicy();

		void build_rendering_buffers(VmaAllocator& allocator, FSlateBatchData& in_batch_data);

		void upload_mesh(VmaAllocator& allocator, const FSlateVertexArray& vertex_array, const FSlateIndexArray& index_array);

		void draw_elements(VkCommandBuffer cmd_buffer, VkPipelineLayout pipeline_layout,const glm::mat4x4& view_projection_matrix, int32_t first_batch_index, const std::vector<FSlateRenderBatch>& render_batches);
	private:
		//template<typename T>
		//void upload_mesh_internal(VmaAllocator& allocator, const T& array);

		FSlateVulkanVertexBuffer m_vertex_buffer;

		FSlateVulkanIndexBuffer m_index_buffer;

		std::shared_ptr<DeletionQueue> m_deletion_queue;
	};
}
