#pragma once

#include "RenderingCommon.h"

#include "SlateCore/Textures/SlateShaderResource.h"

namespace DoDo
{
	//class FSlateVertexArray;
	class FSlateRenderBatch
	{
	public:
		friend class FSlateRenderingPolicy;
		FSlateRenderBatch(
			int32_t in_layer,
			const FShaderParams& in_shader_params,
			const FSlateShaderResource* in_resource,
			ESlateDrawPrimitive in_primitive_type,
			ESlateShader in_shader_type,
			ESlateDrawEffect in_draw_effects,
			FSlateVertexArray* in_source_vertex_array,
			FSlateIndexArray* in_source_index_array,
			int32_t in_vertex_offset,
			int32_t in_index_offset
		);//todo:implement FSlateVertexArray, FSlateIndexArray, InVertexOffset, InIndexOffset

		void reserve_vertices(uint32_t num)
		{
			m_source_vertices->reserve(m_source_vertices->size() + num);//add num
		}

		void reserve_indices(uint32_t num)
		{
			m_source_indices->reserve(m_source_indices->size() + num);//add num
		}

		void add_vertex(FSlateVertex&& vertex)//move
		{
			m_source_vertices->emplace_back(std::forward<FSlateVertex>(vertex));
			++m_num_vertices;
		}

		void add_index(uint16_t index)
		{
			m_source_indices->push_back(index);
			++m_num_indices;
		}

		uint32_t get_num_vertices() const
		{
			return m_num_vertices;
		}

		uint32_t get_num_indices() const
		{
			return m_num_indices;
		}

		uint32_t get_vertex_offset() const
		{
			return m_vertex_offset;
		}

		uint32_t get_index_offset() const
		{
			return m_index_offset;
		}

		bool has_vertex_data()
		{
			return m_num_vertices > 0 && m_num_indices > 0;
		}

		int32_t get_layer() const { return m_layer_id; }

		const FSlateShaderResource* get_shader_resource() const { return m_shader_resource; }

		ESlateDrawEffect get_draw_effects() const { return m_draw_effects; }

		ESlateShader get_shader_type() const { return m_shader_type; }

		const FShaderParams& get_shader_params() const { return m_shader_params; }

		FShaderParams m_shader_params;

		/*shader resource to use with this batch*/
		const FSlateShaderResource* m_shader_resource;

		//source data
		FSlateVertexArray* m_source_vertices;//don't owner the life time of FSlateVertexArray, this is owned by the element batcher

		FSlateIndexArray* m_source_indices;
		/*the layer we need to sort by*/
		int32_t m_layer_id;

		/*number of vertices in the batch*/
		int32_t m_num_vertices;

		/*number of indices in the batch*/
		int32_t m_num_indices;

		int32_t m_vertex_offset;//this is number

		int32_t m_index_offset;//this is number

		int32_t m_next_batch_index;//next batch

		ESlateShader m_shader_type;

		ESlateDrawPrimitive m_draw_primitive_type;

		ESlateDrawEffect m_draw_effects;
	};
}
