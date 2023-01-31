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

	private:
		AllocatedBuffer m_buffer;
	};
}
