#include <PreCompileHeader.h>

#include "SlateVulkanRenderingPolicy.h"

#include "SlateVulkanRenderer.h"
#include "SlateCore/Rendering/ElementBatcher.h"

namespace DoDo
{
	FSlateVulkanRenderingPolicy::FSlateVulkanRenderingPolicy()
	{
		m_deletion_queue = std::make_shared<DeletionQueue>();
	}

	FSlateVulkanRenderingPolicy::~FSlateVulkanRenderingPolicy()
	{
		m_deletion_queue->flush();//flush 
	}

	void FSlateVulkanRenderingPolicy::draw_elements(VkCommandBuffer cmd_buffer, const glm::mat4x4& view_projection_matrix, int32_t first_batch_index,
		const std::vector<FSlateRenderBatch>& render_batches)
	{
		//todo:check vertex buffer and index buffer valid
		if(m_vertex_buffer.m_buffer.m_buffer == nullptr ||
			m_index_buffer.m_buffer.m_buffer == nullptr)
		{
			return;
		}

		//bind index buffer
		//get index offset and draw
		vkCmdBindIndexBuffer(cmd_buffer, m_index_buffer.m_buffer.m_buffer, 0, VkIndexType::VK_INDEX_TYPE_UINT16);

		//set view projection

		int32_t next_render_batch_index = first_batch_index;
		while (next_render_batch_index != -1)//magic number
		{
			const FSlateRenderBatch& render_batch = render_batches[next_render_batch_index];

			next_render_batch_index = render_batch.m_next_batch_index;

			//todo:get render batch information to bind

			const uint32_t offset = render_batch.m_vertex_offset * sizeof(FSlateVertex);

			VkDeviceSize vertex_buffer_offset = offset;

			//bind vertex buffer from offset
			vkCmdBindVertexBuffers(cmd_buffer, 0, 1, &m_vertex_buffer.m_buffer.m_buffer, &vertex_buffer_offset);

			vkCmdDrawIndexed(cmd_buffer, render_batch.m_num_indices, 1, render_batch.m_index_offset, 0, 0);
		}
	}

	void FSlateVulkanRenderingPolicy::build_rendering_buffers(VmaAllocator& allocator, FSlateBatchData& in_batch_data)
	{
		//todo:implement merge rendering batches for FSlateBatchData, this function will sort render batch and get the next batch index

		//todo:implement interms of current size to resize

		if(!in_batch_data.get_render_batches().empty())
		{
			//todo:get the batch data bached data to generate buffer

			const FSlateVertexArray& vertex_array = in_batch_data.get_vertex_data();
			const FSlateIndexArray& index_array = in_batch_data.get_index_data();

			//upload buffer to vulkan memory
			upload_mesh(allocator, vertex_array, index_array);
		}
	}

	void FSlateVulkanRenderingPolicy::upload_mesh(VmaAllocator& allocator, const FSlateVertexArray& vertex_array,
		const FSlateIndexArray& index_array)
	{
		//---------------------------Vertex Buffer Create---------------------------
		//allocate vertex buffer
		VkBufferCreateInfo buffer_info = {};

		buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		//this is the total size, in bytes, of the buffer we are allocating
		buffer_info.size = vertex_array.size() * sizeof(FSlateVertex);
		//this buffer is going to be used as a vertex buffer
		buffer_info.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

		//let the vma library know that this data should be writeable by cpu, but also readable by gpu
		VmaAllocationCreateInfo vma_allocation_info = {};
		vma_allocation_info.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

		//allocate the buffer
		vmaCreateBuffer(allocator, &buffer_info, &vma_allocation_info, &m_vertex_buffer.m_buffer.m_buffer, &m_vertex_buffer.m_buffer.m_allocation, nullptr);

		//add the destruction of triangle mesh buffer to the deletion queue
		m_deletion_queue->push_function([=]()
		{
			vmaDestroyBuffer(allocator, m_vertex_buffer.m_buffer.m_buffer, m_vertex_buffer.m_buffer.m_allocation);
		});

		//copy cpu to gpu
		void* data;
		vmaMapMemory(allocator, m_vertex_buffer.m_buffer.m_allocation, &data);

		memcpy(data, vertex_array.data(), vertex_array.size() * sizeof(FSlateVertex));

		vmaUnmapMemory(allocator, m_vertex_buffer.m_buffer.m_allocation);
		//---------------------------Vertex Buffer Create---------------------------

		//---------------------------Index Buffer Create---------------------------

		//allocate vertex buffer
		buffer_info = {};

		buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		//this is the total size, in bytes, of the buffer we are allocating
		buffer_info.size = index_array.size() * sizeof(uint16_t);
		//this buffer is going to be used as a vertex buffer
		buffer_info.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

		//let the vma library know that this data should be writeable by cpu, but also readable by gpu
		vma_allocation_info = {};
		vma_allocation_info.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

		//allocate the buffer
		vmaCreateBuffer(allocator, &buffer_info, &vma_allocation_info, &m_index_buffer.m_buffer.m_buffer, &m_index_buffer.m_buffer.m_allocation, nullptr);

		//add the destruction of triangle mesh buffer to the deletion queue
		m_deletion_queue->push_function([=]()
		{
			vmaDestroyBuffer(allocator, m_index_buffer.m_buffer.m_buffer, m_index_buffer.m_buffer.m_allocation);
		});

		//copy cpu to gpu
		vmaMapMemory(allocator, m_index_buffer.m_buffer.m_allocation, &data);

		memcpy(data, index_array.data(), index_array.size() * sizeof(uint16_t));

		vmaUnmapMemory(allocator, m_index_buffer.m_buffer.m_allocation);

		//---------------------------Index Buffer Create---------------------------
	}
}
