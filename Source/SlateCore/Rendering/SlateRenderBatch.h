#pragma once

#include "RenderingCommon.h"

namespace DoDo
{
	//class FSlateVertexArray;
	class FSlateRenderBatch
	{
	public:
		FSlateRenderBatch(
			int32_t in_layer,
			ESlateDrawPrimitive in_primitive_type,
			ESlateDrawEffect in_draw_effects,
			FSlateVertexArray* in_source_vertex_array
		);//todo:implement FSlateVertexArray, FSlateIndexArray, InVertexOffset, InIndexOffset

		void add_vertex(FSlateVertex&& vertex)//move
		{
			m_source_vertices->emplace_back(std::forward<FSlateVertex>(vertex));
			++m_num_vertices;
		}
	private:
		//source data
		FSlateVertexArray* m_source_vertices;//don't owner the life time of FSlateVertexArray, this is owned by the element batcher
		/*the layer we need to sort by*/
		int32_t m_layer_id;

		/*number of vertices in the batch*/
		int32_t m_num_vertices;

		/*number of indices in the batch*/
		int32_t m_num_indices;

		ESlateDrawPrimitive m_draw_primitive_type;

		ESlateDrawEffect m_draw_effects;
	};
}
