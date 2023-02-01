#pragma once

#include "RenderingCommon.h"

namespace DoDo
{
	//class FSlateVertexArray;
	class FSlateRenderBatch
	{
	public:
		friend class FSlateRenderingPolicy;
		FSlateRenderBatch(
			int32_t in_layer,
			ESlateDrawPrimitive in_primitive_type,
			ESlateDrawEffect in_draw_effects,
			FSlateVertexArray* in_source_vertex_array,
			FSlateIndexArray* in_source_index_array,
			int32_t in_vertex_offset,
			int32_t in_index_offset
		);//todo:implement FSlateVertexArray, FSlateIndexArray, InVertexOffset, InIndexOffset

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

		//source data
		FSlateVertexArray* m_source_vertices;//don't owner the life time of FSlateVertexArray, this is owned by the element batcher

		FSlateIndexArray* m_source_indices;
		/*the layer we need to sort by*/
		int32_t m_layer_id;

		/*number of vertices in the batch*/
		int32_t m_num_vertices;

		/*number of indices in the batch*/
		int32_t m_num_indices;

		int32_t m_vertex_offset;

		int32_t m_index_offset;

		int32_t m_next_batch_index;//next batch

		ESlateDrawPrimitive m_draw_primitive_type;

		ESlateDrawEffect m_draw_effects;
	};
}
