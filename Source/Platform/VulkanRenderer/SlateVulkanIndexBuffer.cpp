#include <PreCompileHeader.h>

#include "SlateVulkanIndexBuffer.h"

namespace DoDo
{
	FSlateVulkanIndexBuffer::FSlateVulkanIndexBuffer()
		: m_max_num_indices(0)
	{
	}

	FSlateVulkanIndexBuffer::~FSlateVulkanIndexBuffer()
	{
	}
	void FSlateVulkanIndexBuffer::create_buffer(VmaAllocator& allocator)
	{
		if (m_max_num_indices == 0)
		{
			m_max_num_indices = 1000;
		}

		uint32_t size_bytes = m_max_num_indices * sizeof(uint16_t);

		upload_mesh(allocator, size_bytes);
	}

	void FSlateVulkanIndexBuffer::upload_mesh(VmaAllocator& allocator, uint32_t size)
	{
		//---------------------------Vertex Buffer Create---------------------------
		//allocate vertex buffer
		VkBufferCreateInfo buffer_info = {};

		buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		//this is the total size, in bytes, of the buffer we are allocating
		buffer_info.size = size;
		//this buffer is going to be used as a vertex buffer
		buffer_info.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;

		//let the vma library know that this data should be writeable by cpu, but also readable by gpu
		VmaAllocationCreateInfo vma_allocation_info = {};
		vma_allocation_info.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

		//allocate the buffer
		vmaCreateBuffer(allocator, &buffer_info, &vma_allocation_info, &m_buffer.m_buffer, &m_buffer.m_allocation, nullptr);

		//copy cpu to gpu
		//void* data;
		//vmaMapMemory(allocator, m_vertex_buffer.m_buffer.m_allocation, &data);
		//
		//memcpy(data, vertex_array.data(), vertex_array.size() * sizeof(FSlateVertex));
		//
		//vmaUnmapMemory(allocator, m_vertex_buffer.m_buffer.m_allocation);
		//---------------------------Vertex Buffer Create---------------------------
	}

	void FSlateVulkanIndexBuffer::destroy_buffer(VmaAllocator& allocator)
	{
		vmaDestroyBuffer(allocator, m_buffer.m_buffer, m_buffer.m_allocation);
	}

	void* FSlateVulkanIndexBuffer::lock(VmaAllocator& allocator, uint32_t offset)
	{
		void* data;
		vmaMapMemory(allocator, m_buffer.m_allocation, &data);

		//memcpy(data, vertex_array.data(), vertex_array.size() * sizeof(FSlateVertex));

		//vmaUnmapMemory(allocator, m_vertex_buffer.m_buffer.m_allocation);
		return (void*)((uint8_t*)data + offset);//todo:one byte offset
	}
	void FSlateVulkanIndexBuffer::unlock(VmaAllocator& allocator)
	{
		vmaUnmapMemory(allocator, m_buffer.m_allocation);
	}

	void FSlateVulkanIndexBuffer::resize_buffer(VkDevice& device, VmaAllocator& allocator, uint32_t num_indices)
	{
		//buffer should be created first

		if (num_indices > m_max_num_indices)
		{
			//todo:first to copy the already have buffer's data, then copy to new buffer

			uint32_t buffer_size = m_max_num_indices * sizeof(uint16_t);

			uint8_t* saved_indices = nullptr;

			{
				void* indices = lock(allocator, 0);

				saved_indices = new uint8_t[m_max_num_indices * sizeof(uint16_t)];
				std::memcpy(saved_indices, indices, buffer_size);

				unlock(allocator);
			}

			vkDeviceWaitIdle(device);

			vmaDestroyBuffer(allocator, m_buffer.m_buffer, m_buffer.m_allocation);
			if (saved_indices)
			{
				//copy to new buffer
				upload_mesh(allocator, num_indices * sizeof(uint16_t));//create new buffer

				void* indices = lock(allocator, 0);

				std::memcpy(indices, saved_indices, buffer_size);

				unlock(allocator);

				m_max_num_indices = num_indices;
			}

			delete[] saved_indices;
		}
	}
}
