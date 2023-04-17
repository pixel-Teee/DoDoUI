#pragma once

#include "Utils.h"

namespace DoDo
{
	class FSlateVulkanIndexBuffer
	{
	public:
		friend class FSlateVulkanRenderingPolicy;

		FSlateVulkanIndexBuffer();
		virtual ~FSlateVulkanIndexBuffer();

		void create_buffer(VmaAllocator& allocator);

		void upload_mesh(VmaAllocator& allocator, uint32_t size);

		void destroy_buffer(VmaAllocator& allocator);

		void* lock(VmaAllocator& allocator, uint32_t offset);

		void unlock(VmaAllocator& allocator);

		void resize_buffer(VkDevice& device, VmaAllocator& allocator, uint32_t num_indices);

		uint32_t get_max_num_indices() const { return m_max_num_indices; }

	private:
		uint32_t m_max_num_indices;

		AllocatedBuffer m_buffer;
	};
}
