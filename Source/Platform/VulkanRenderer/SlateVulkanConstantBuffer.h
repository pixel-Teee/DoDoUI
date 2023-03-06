#pragma once

#include "Utils.h"

namespace DoDo {
	template<typename BufferType>
	class FSlateVulkanConstantBuffer
	{
	public:
		void create(VmaAllocator& allocator)
		{
			VkBufferCreateInfo buffer_info = {};

			buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;

			buffer_info.size = sizeof(BufferType);

			buffer_info.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

			VmaAllocationCreateInfo vma_allocation_info = {};
			vma_allocation_info.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

			//allocate the buffer
			vmaCreateBuffer(allocator, &buffer_info, &vma_allocation_info, &m_buffer.m_buffer, &m_buffer.m_allocation, nullptr);

			m_allocator = allocator;
		}

		void destroy_buffer(VmaAllocator& allocator)
		{
			vmaDestroyBuffer(allocator, m_buffer.m_buffer, m_buffer.m_allocation);
		}

		AllocatedBuffer& get_resource() { return m_buffer; }//todo:fix me

		BufferType& get_buffer_data() { return m_buffer_data; }

		void update_buffer()
		{
			void* data = lock();
			std::memcpy(data, &m_buffer_data, sizeof(m_buffer_data));
			un_lock();
		}

		void* lock()
		{
			void* data;
			vmaMapMemory(m_allocator, m_buffer.m_allocation, &data);

			//memcpy(data, vertex_array.data(), vertex_array.size() * sizeof(FSlateVertex));

			//vmaUnmapMemory(allocator, m_vertex_buffer.m_buffer.m_allocation);
			return data;//todo:one byte offset
		}

		void un_lock()
		{
			vmaUnmapMemory(m_allocator, m_buffer.m_allocation);
		}
	private:
		AllocatedBuffer m_buffer;

		VmaAllocator m_allocator;//todo:fix me

		BufferType m_buffer_data;
	};
}