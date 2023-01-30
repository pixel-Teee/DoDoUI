#include <PreCompileHeader.h>

#include "SlateRenderBatch.h"

namespace DoDo
{
	FSlateRenderBatch::FSlateRenderBatch(int32_t in_layer, ESlateDrawPrimitive in_primitive_type,
		ESlateDrawEffect in_draw_effects, FSlateVertexArray* in_source_vertex_array)
			: m_layer_id(in_layer)
			, m_draw_primitive_type(in_primitive_type)
			, m_draw_effects(in_draw_effects)
			, m_source_vertices(in_source_vertex_array)
	{
	}
}
