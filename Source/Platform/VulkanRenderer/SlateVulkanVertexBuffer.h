#pragma once

#include "Utils.h"

namespace DoDo
{
	/*
	 * vertex buffer containing all slate vertices
	 */
	class FSlateVulkanVertexBuffer
	{
	public:
		friend class FSlateVulkanRenderingPolicy;

		FSlateVulkanVertexBuffer();

		~FSlateVulkanVertexBuffer();

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