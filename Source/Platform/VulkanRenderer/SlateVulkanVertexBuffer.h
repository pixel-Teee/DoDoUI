#pragma once

#include "Utils.h"

#include "SlateCore/Rendering/RenderingCommon.h"

namespace DoDo
{
	//class FSlateIndexArray;
	//class FSlateVertexArray;
	/*
	 * vertex buffer containing all slate vertices
	 */
	class FSlateVulkanVertexBuffer
	{
	public:
		friend class FSlateVulkanRenderingPolicy;

		FSlateVulkanVertexBuffer();

		~FSlateVulkanVertexBuffer();

		void create_buffer(VmaAllocator& allocator, uint32_t in_stride);

		void destroy_buffer(VmaAllocator& allocator);

		void FSlateVulkanVertexBuffer::upload_mesh(VmaAllocator& allocator, uint32_t size);

		void* lock(VmaAllocator& allocator, uint32_t offset);
		
		void unlock(VmaAllocator& allocator);

		void resize_buffer(VmaAllocator& allocator, uint32_t new_size);

		uint32_t get_buffer_size() const { return m_buffer_size; }

	private:
		/*the size of the buffer in bytes*/
		uint32_t m_buffer_size;

		uint32_t m_stride;

		AllocatedBuffer m_buffer;//vulkan buffer

		/*hidden copy methods*/
		FSlateVulkanVertexBuffer(const FSlateVulkanVertexBuffer&);
		void operator=(const FSlateVulkanVertexBuffer&);
	};
}