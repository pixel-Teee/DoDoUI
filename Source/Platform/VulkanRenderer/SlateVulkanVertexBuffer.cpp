#include <PreCompileHeader.h>

#include "SlateVulkanVertexBuffer.h"

namespace DoDo
{
	FSlateVulkanVertexBuffer::FSlateVulkanVertexBuffer()
		: m_buffer_size(0)
		, m_stride(0)
	{
	}

	FSlateVulkanVertexBuffer::~FSlateVulkanVertexBuffer()
	{
	}

	void FSlateVulkanVertexBuffer::create_buffer(VmaAllocator& allocator, uint32_t in_stride)
	{
		m_stride = in_stride;

		if (m_buffer_size == 0)
		{
			//todo : better default size
			m_buffer_size = 500 * in_stride;
		}

		upload_mesh(allocator, m_buffer_size);
	}

	void FSlateVulkanVertexBuffer::destroy_buffer(VmaAllocator& allocator)
	{
		vmaDestroyBuffer(allocator, m_buffer.m_buffer, m_buffer.m_allocation);
	}

	void FSlateVulkanVertexBuffer::upload_mesh(VmaAllocator& allocator, uint32_t size)
	{
		//---------------------------Vertex Buffer Create---------------------------
		//allocate vertex buffer
		VkBufferCreateInfo buffer_info = {};

		buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		//this is the total size, in bytes, of the buffer we are allocating
		buffer_info.size = size;
		//this buffer is going to be used as a vertex buffer
		buffer_info.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

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
	void* FSlateVulkanVertexBuffer::lock(VmaAllocator& allocator, uint32_t offset)
	{
		void* data;
		vmaMapMemory(allocator, m_buffer.m_allocation, &data);

		//memcpy(data, vertex_array.data(), vertex_array.size() * sizeof(FSlateVertex));

		//vmaUnmapMemory(allocator, m_vertex_buffer.m_buffer.m_allocation);
		return (void*)((uint8_t*)data + offset);//todo:one byte offset
	}
	void FSlateVulkanVertexBuffer::unlock(VmaAllocator& allocator)
	{
		vmaUnmapMemory(allocator, m_buffer.m_allocation);
	}

	/*resize the buffer to the passed in size, preserves internal data*/
	void FSlateVulkanVertexBuffer::resize_buffer(VkDevice& device, VmaAllocator& allocator, uint32_t new_size)
	{
		//buffer should be created first

		if (new_size > m_buffer_size)
		{
			//todo:first to copy the already have buffer's data, then copy to new buffer

			uint8_t* saved_vertices = nullptr;

			{
				void* vertices = lock(allocator, 0);

				saved_vertices = new uint8_t[m_buffer_size];
				std::memcpy(saved_vertices, vertices, m_buffer_size);

				unlock(allocator);
			}

			vkDeviceWaitIdle(device);//todo:fix me

			vmaDestroyBuffer(allocator, m_buffer.m_buffer, m_buffer.m_allocation);
			if (saved_vertices)
			{
				//copy to new buffer
				upload_mesh(allocator, new_size);//create new buffer

				void* vertices = lock(allocator, 0);

				std::memcpy(vertices, saved_vertices, m_buffer_size);

				unlock(allocator);

				m_buffer_size = new_size;
			}

			delete[] saved_vertices;
		}
	}
}
