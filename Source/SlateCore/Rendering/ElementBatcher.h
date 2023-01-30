#pragma once

#include "DrawElementPayloads.h"//FSlateBoxPayload
#include "RenderingCommon.h"
#include "SlateRenderBatch.h"

#include "glm/glm.hpp"

namespace DoDo
{
	class FSlateRenderBatch;//create render batch depends on it
	class FSlateWindowElementList;

	class FSlateBatchData
	{
	public:
		FSlateRenderBatch& add_render_batch(
			int32_t in_layer,
			ESlateDrawPrimitive in_primitive_type,
			ESlateDrawEffect in_draw_effects
		);
	private:
		//todo:implement FSlateVertexArray and other information
		std::vector<FSlateRenderBatch> m_render_batches;

		/*uncached source vertices and indices from volatile elements*/
		FSlateVertexArray m_uncached_source_batch_vertices;

		FSlateVertexArray m_uncached_source_batch_indices;
	};

	/*
	 * a class which batches slate elements for rendering
	 */
	class FSlateElementBatcher
	{
	public:

		/*
		 * batches elements to be rendered
		 *
		 * @param DrawElements the elements to batch
		 */
		void add_elements(FSlateWindowElementList& element_list);//this is important, is entry for generate vertex and index

	private:
		void add_elements_internal(const FSlateDrawElementArray& draw_elements, const glm::vec2& view_port_size);

		/*
		 * creates vertices necessary to draw a 3x3 element
		 */
		template<ESlateVertexRounding rounding>
		void add_box_element(const FSlateDrawElement& draw_element);

		FSlateRenderBatch& create_render_batch(
			int32_t layer,
			ESlateDrawPrimitive primitive_type,//triangle or line
			ESlateDrawEffect draw_effects,
			const FSlateDrawElement& draw_element
		);

	private:
		/*uncached batch data currently being filled in*/
		FSlateBatchData* m_batch_data;
	};

	
}
