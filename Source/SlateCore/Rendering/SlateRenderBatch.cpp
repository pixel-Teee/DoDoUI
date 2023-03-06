#include <PreCompileHeader.h>

#include "SlateRenderBatch.h"

namespace DoDo
{
	FSlateRenderBatch::FSlateRenderBatch(int32_t in_layer, const FShaderParams& in_shader_params, const FSlateShaderResource* in_resource, ESlateDrawPrimitive in_primitive_type,
		ESlateDrawEffect in_draw_effects, FSlateVertexArray* in_source_vertex_array, FSlateIndexArray* in_source_index_array, int32_t in_vertex_offset,
		int32_t in_index_offset)
			: m_shader_params(in_shader_params)
		    , m_layer_id(in_layer)
			, m_shader_resource(in_resource)
			, m_draw_primitive_type(in_primitive_type)
			, m_draw_effects(in_draw_effects)
			, m_source_vertices(in_source_vertex_array)
			, m_source_indices(in_source_index_array)
			, m_vertex_offset(in_vertex_offset)
			, m_index_offset(in_index_offset)
			, m_num_vertices(0)
			, m_num_indices(0)
			, m_next_batch_index(-1)//batch index
	{
	}
}
