#pragma once

#include "DrawElementPayloads.h"//FSlateBoxPayload
#include "RenderingCommon.h"
#include "SlateRenderBatch.h"

#include "glm/glm.hpp"

namespace DoDo
{
	class FSlateRenderBatch;//create render batch depends on it
	class FSlateWindowElementList;
	class FSlateRenderingPolicy;

	class FSlateBatchData
	{
	public:
		FSlateBatchData()
			: m_first_render_batch_index(-1)//todo:modify this to -1
			, m_num_layers(0)
			, m_num_batches(0)
			, m_total_vertex_offset(0)
			, m_total_index_offset(0)
		{}

		FSlateRenderBatch& add_render_batch(
			int32_t in_layer,
			const FShaderParams& in_shader_params,
			FSlateShaderResource* shader_resource,
			ESlateDrawPrimitive in_primitive_type,
			ESlateShader in_shader_type,
			ESlateDrawEffect in_draw_effects
		);

		void reset_data();

		/*
		 * creates rendering data from batched elements
		 */
		void merge_render_batches();

		/*
		 * returns a list of element batches for this window
		 */
		const std::vector<FSlateRenderBatch>& get_render_batches() const { return m_render_batches; }

		int32_t get_first_render_batch_index() const { return m_first_render_batch_index; }

		const FSlateVertexArray& get_vertex_data() const { return m_uncached_source_batch_vertices; }
		const FSlateIndexArray& get_index_data() const { return m_uncached_source_batch_indices; }

		const FSlateVertexArray& get_final_vertex_data() const { return m_final_vertex_data; }
		const FSlateIndexArray& get_final_index_data() const { return m_final_index_data; }

		void set_total_vertex_offset(uint32_t offset) { m_total_vertex_offset = offset; }
		void set_total_index_offset(uint32_t offset) { m_total_index_offset = offset; }

		uint32_t get_total_vertex_offset() const { return m_total_vertex_offset; }
		uint32_t get_total_index_offset() const { return m_total_index_offset; }

		//todo:to implement merge render batches
	protected:
		void fill_buffers_from_new_batch(FSlateRenderBatch& batch, FSlateVertexArray& final_vertices, FSlateIndexArray& final_indices);

	private:
		//todo:implement FSlateVertexArray and other information
		std::vector<FSlateRenderBatch> m_render_batches;

		int32_t m_first_render_batch_index;

		/*uncached source vertices and indices from volatile elements*/
		FSlateVertexArray m_uncached_source_batch_vertices;

		FSlateIndexArray m_uncached_source_batch_indices;

		FSlateVertexArray m_final_vertex_data;

		FSlateIndexArray m_final_index_data;

		int32_t m_num_layers;

		/*number of final render batches, it is not the same as RenderBatches.Num()*/
		int32_t m_num_batches;

		uint32_t m_total_vertex_offset;//todo:this record video memory buffer offset since last window element list

		uint32_t m_total_index_offset;
	};

	/*
	 * a class which batches slate elements for rendering
	 */
	class FSlateElementBatcher
	{
	public:
		FSlateElementBatcher(std::shared_ptr<FSlateRenderingPolicy> in_rendering_policy);

		~FSlateElementBatcher();
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

		/*
		 * creates vertices necessary to draw a string (one quad per character)
		 */
		template<ESlateVertexRounding rounding>
		void add_text_element(const FSlateDrawElement& draw_element);

		/*
		 * creates vertices necessary to draw a gradient box (horizontal or vertical)
		 */
		template<ESlateVertexRounding Rounding>
		void add_gradient_element(const FSlateDrawElement& draw_element);

		FSlateRenderBatch& create_render_batch(
			int32_t layer,
			const FShaderParams& shader_params,
			FSlateShaderResource* shader_resource,
			ESlateDrawPrimitive primitive_type,//triangle or line
			ESlateShader in_shader_type,
			ESlateDrawEffect draw_effects,
			const FSlateDrawElement& draw_element
		);

	private:
		/*uncached batch data currently being filled in*/
		FSlateBatchData* m_batch_data;//life time owns by the FSlateWindowElementList

		/*rendering policy we were created from*/
		FSlateRenderingPolicy* m_rendering_policy;
	};

	
}
