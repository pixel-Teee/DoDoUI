#include <PreCompileHeader.h>

#include "SlateVulkanRenderingPolicy.h"

#include "SlateVulkanRenderer.h"
#include "SlateCore/Rendering/ElementBatcher.h"

#include "VulkanInitializers.h"

#include "SlateVulkanTextures.h"

namespace DoDo
{
	FSlateVulkanRenderingPolicy::FSlateVulkanRenderingPolicy(VmaAllocator& allocator)
	{
		m_deletion_queue = std::make_shared<DeletionQueue>();

		m_vertex_buffer.create_buffer(allocator, sizeof(FSlateVertex));
		m_index_buffer.create_buffer(allocator);
		m_last_vertex_buffer_offset = 0;
		m_last_index_buffer_offset = 0;

		m_shader_resource = nullptr;
	}

	FSlateVulkanRenderingPolicy::~FSlateVulkanRenderingPolicy()
	{
		m_deletion_queue->flush();//flush 

		//m_vertex_buffer.destroy_buffer(allocator);
		//m_index_buffer.destroy_buffer();
		//todo:delete buffer
	}

	void FSlateVulkanRenderingPolicy::clear_vulkan_buffer(VmaAllocator& allocator)
	{
		m_deletion_queue->flush();//flush

		m_vertex_buffer.destroy_buffer(allocator);
		m_index_buffer.destroy_buffer(allocator);
	}

	void FSlateVulkanRenderingPolicy::reset_offset()
	{
		m_last_index_buffer_offset = m_last_vertex_buffer_offset = 0;
	}

	void FSlateVulkanRenderingPolicy::draw_elements(VkDevice device, VkDescriptorSet descriptor_set, VkCommandBuffer cmd_buffer, VkPipelineLayout pipeline_layout, VkSampler sampler, const glm::mat4x4& view_projection_matrix, int32_t first_batch_index,
	                                                const std::vector<FSlateRenderBatch>& render_batches, uint32_t total_vertex_offset, uint32_t total_index_offset)
	{
		//todo:check vertex buffer and index buffer valid
		if(m_vertex_buffer.m_buffer.m_buffer == VK_NULL_HANDLE ||
			m_index_buffer.m_buffer.m_buffer == VK_NULL_HANDLE)
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

			const FSlateShaderResource* shader_resource = render_batch.get_shader_resource();//todo:shader resource is image view

			//todo:get render batch information to bind
			if (shader_resource != nullptr && shader_resource != m_shader_resource)
			{			
				m_shader_resource = const_cast<FSlateShaderResource*>(shader_resource);
				//------update descriptor set------
				VkDescriptorImageInfo imageBufferInfo;
				imageBufferInfo.sampler = sampler;
				imageBufferInfo.imageView = ((FSlateVulkanTexture*)shader_resource)->get_typed_resource();//get the image view
				imageBufferInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

				VkWriteDescriptorSet texture1 = write_descriptor_image(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, descriptor_set, &imageBufferInfo, 1);

				vkUpdateDescriptorSets(device, 1, &texture1, 0, nullptr);
				//------update descriptor set------		
			}	

			//texture descriptor
			vkCmdBindDescriptorSets(cmd_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_layout, 0, 1, &descriptor_set, 0, nullptr);

			const uint32_t offset = render_batch.m_vertex_offset * sizeof(FSlateVertex) + total_vertex_offset;

			VkDeviceSize vertex_buffer_offset = offset;

			//push constants
			vkCmdPushConstants(cmd_buffer, pipeline_layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(glm::mat4), &view_projection_matrix);

			//note:this vertex offset is bytes
			//bind vertex buffer from offset
			vkCmdBindVertexBuffers(cmd_buffer, 0, 1, &m_vertex_buffer.m_buffer.m_buffer, &vertex_buffer_offset);

			//note:this index buffer offset is number
			vkCmdDrawIndexed(cmd_buffer, render_batch.m_num_indices, 1, render_batch.m_index_offset + total_index_offset / sizeof(uint16_t), 0, 0);
		}
	}

	void FSlateVulkanRenderingPolicy::build_rendering_buffers(VmaAllocator& allocator, FSlateBatchData& in_batch_data)
	{
		//todo:implement merge rendering batches for FSlateBatchData, this function will sort render batch and get the next batch index
		in_batch_data.merge_render_batches();
		//todo:implement interms of current size to resize
		//m_deletion_queue->flush();//todo:remove this

		in_batch_data.set_total_vertex_offset(m_last_vertex_buffer_offset);
		in_batch_data.set_total_index_offset(m_last_index_buffer_offset);

		if(!in_batch_data.get_render_batches().empty())
		{
			//todo:get the batch data bached data to generate buffer

			const FSlateVertexArray& final_vertex_data = in_batch_data.get_final_vertex_data();
			const FSlateIndexArray& final_index_data = in_batch_data.get_final_index_data();

			if (!final_vertex_data.empty())
			{
				const uint32_t num_vertices = final_vertex_data.size();

				//resize if needed
				if (num_vertices * sizeof(FSlateVertex) + m_last_vertex_buffer_offset > m_vertex_buffer.get_buffer_size())
				{
					uint32_t num_bytes_needed = (num_vertices + m_last_vertex_buffer_offset) * sizeof(FSlateVertex);
					m_vertex_buffer.resize_buffer(allocator, num_bytes_needed + (num_vertices / 4) * sizeof(FSlateVertex));// extra 1/4 space
				}
			}

			if (!final_index_data.empty())
			{
				const uint32_t num_indices = final_index_data.size();

				const uint32_t last_offset = m_last_index_buffer_offset / sizeof(uint16_t);

				if (num_indices + last_offset > m_index_buffer.get_max_num_indices())
				{
					m_index_buffer.resize_buffer(allocator, num_indices + last_offset + (num_indices / 4));
				}
			}

			//map and copy
			uint8_t* vertices_ptr = nullptr;
			uint8_t* indices_ptr = nullptr;
			{
				vertices_ptr = (uint8_t*)m_vertex_buffer.lock(allocator, m_last_vertex_buffer_offset);
				indices_ptr = (uint8_t*)m_index_buffer.lock(allocator, m_last_index_buffer_offset);
			}

			std::memcpy(vertices_ptr, final_vertex_data.data(), final_vertex_data.size() * sizeof(FSlateVertex));
			std::memcpy(indices_ptr, final_index_data.data(), final_index_data.size() * sizeof(uint16_t));

			m_vertex_buffer.unlock(allocator);
			m_index_buffer.unlock(allocator);

			m_last_vertex_buffer_offset += final_vertex_data.size() * sizeof(FSlateVertex);
			m_last_index_buffer_offset += final_index_data.size() * sizeof(uint16_t);


			//upload buffer to vulkan memory
			//upload_mesh(allocator, vertex_array, index_array);
		}
	}

	/*
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
		buffer_info.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;

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
	*/
}
