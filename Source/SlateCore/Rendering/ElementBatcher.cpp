#include <PreCompileHeader.h>

#include "ElementBatcher.h"

#include "DrawElements.h"
#include "SlateRenderBatch.h"
#include "glm/vec2.hpp"
#include "SlateCore/Widgets/SWindow.h"

namespace DoDo
{
	FSlateRenderBatch& FSlateBatchData::add_render_batch(int32_t in_layer, FSlateShaderResource* shader_resource, ESlateDrawPrimitive in_primitive_type,
		ESlateDrawEffect in_draw_effects)
	{
		return m_render_batches.emplace_back(in_layer, shader_resource, in_primitive_type, in_draw_effects, &m_uncached_source_batch_vertices, &m_uncached_source_batch_indices, m_uncached_source_batch_vertices.size(), m_uncached_source_batch_indices.size());
	}

	void FSlateBatchData::reset_data()
	{
		m_render_batches.clear();
		m_uncached_source_batch_vertices.clear();
		m_uncached_source_batch_indices.clear();
		m_final_vertex_data.clear();
		m_final_index_data.clear();

		m_total_vertex_offset = m_total_index_offset = 0;

		m_first_render_batch_index = -1;

		m_num_layers = 0;
		m_num_batches = 0;
	}

	void FSlateBatchData::merge_render_batches()
	{
		if(m_render_batches.size())
		{
			std::vector<std::pair<int32_t, int32_t>> batch_indices;

			{
				//sort an index array instead of the render batches since they are large and not trivially relocatable
				batch_indices.resize(m_render_batches.size());

				for(int32_t index = 0; index < m_render_batches.size(); ++index)
				{
					batch_indices[index].first = index;
					batch_indices[index].second = m_render_batches[index].get_layer();
				}

				//stable sort because order in the same layer should be preserved
				std::stable_sort(batch_indices.begin(), batch_indices.end(),
					[](const std::pair<int32_t, int32_t>& a, const std::pair<int32_t, int32_t>& b)
				{
					return a.second < b.second;
				});
			}

			m_num_batches = 0;
			m_num_layers = 0;

			m_first_render_batch_index = batch_indices[0].first;

			FSlateRenderBatch* prev_batch = nullptr;
			for(int32_t batch_index = 0; batch_index < batch_indices.size(); ++batch_index)
			{
				const std::pair<int32_t, int32_t>& batch_index_pair = batch_indices[batch_index];

				FSlateRenderBatch& current_batch = m_render_batches[batch_index_pair.first];

				if(prev_batch != nullptr)
				{
					prev_batch->m_next_batch_index = batch_index_pair.first;
				}

				++m_num_batches;

				//todo:implement fill buffers from new batch
				fill_buffers_from_new_batch(current_batch, m_final_vertex_data, m_final_index_data);

				//todo:temporarily do nothing

				prev_batch = &current_batch;
			}
		}
	}

	void FSlateBatchData::fill_buffers_from_new_batch(FSlateRenderBatch& batch, FSlateVertexArray& final_vertices,
		FSlateIndexArray& final_indices)
	{
		if(batch.has_vertex_data())
		{
			const int32_t source_vertex_offset = batch.m_vertex_offset;
			const int32_t source_index_offset = batch.m_index_offset;//from another array to copy to this array

			batch.m_vertex_offset = final_vertices.size();
			batch.m_index_offset = final_indices.size();

			m_final_vertex_data.insert(m_final_vertex_data.end(), (*batch.m_source_vertices).begin() + source_vertex_offset, (*batch.m_source_vertices).begin() + source_vertex_offset + batch.m_num_vertices);
			m_final_index_data.insert(m_final_index_data.end(), (*batch.m_source_indices).begin() + source_index_offset, (*batch.m_source_indices).begin() + source_index_offset + batch.m_num_indices);
		}
	}

	void FSlateElementBatcher::add_elements(FSlateWindowElementList& element_list)
	{
		//todo:implement viewport size
		//glm::vec2 view_port_size = element_list.get_paint_window();
		m_batch_data = &element_list.get_batch_data();//get the batch data from element list, don't owner the life time of batch data

		glm::vec2 view_port_size = element_list.get_paint_window()->get_view_port_size();//if this is 0, will get the client size

		add_elements_internal(element_list.get_uncached_draw_elements(), view_port_size);
	}

	void FSlateElementBatcher::add_elements_internal(const FSlateDrawElementArray& draw_elements,
		const glm::vec2& view_port_size)
	{
		for(const FSlateDrawElement& draw_element : draw_elements)
		{
			//determine what type of element to add
			switch(draw_element.get_element_type())
			{
			case EElementType::ET_Box:
			case EElementType::ET_RoundedBox:
			{
				//todo:impement pixel snapped
				add_box_element<ESlateVertexRounding::Disabled>(draw_element);
			}
			}
		}
	}

	FSlateRenderBatch& FSlateElementBatcher::create_render_batch(int32_t layer, FSlateShaderResource* shader_resource, ESlateDrawPrimitive primitive_type,
		ESlateDrawEffect draw_effects, const FSlateDrawElement& draw_element)
	{
		//render batch store the vertex and index data
		FSlateRenderBatch& new_batch = m_batch_data->add_render_batch(layer, shader_resource, primitive_type, draw_effects);

		return new_batch;
	}

	template <ESlateVertexRounding rounding>
	void FSlateElementBatcher::add_box_element(const FSlateDrawElement& draw_element)
	{
		//cast to specific type
		//note:box pay load is just some draw related information(don't contain the information about the geometry)
		const FSlateBoxPayload& draw_element_pay_load = draw_element.get_data_pay_load<FSlateBoxPayload>();

		//todo:implement from linear color to srgb color
		const glm::vec4 tint = draw_element_pay_load.get_tint();

		const FSlateRenderTransform& element_render_transform = draw_element.get_render_transform();
		const FSlateRenderTransform render_transform = draw_element.get_render_transform();

		const glm::vec2 local_size = draw_element.get_local_size();

		const ESlateDrawEffect in_draw_effects = draw_element.get_draw_effects();

		const int32_t layer = draw_element.get_layer();

		const float draw_scale = draw_element.get_scale();

		//todo:do pixel snapping
		glm::vec2 top_left(0.0f, 0.0f);
		glm::vec2 bottom_right(local_size);

		uint32_t texture_width = 1;
		uint32_t texture_height = 1;

		//get the default start and end uv, if the texture s atlased this value will be a subset of this
		glm::vec2 start_uv = glm::vec2(0.0f, 0.0f);
		glm::vec2 end_uv = glm::vec2(1.0f, 1.0f);
		glm::vec2 size_uv;

		glm::vec2 half_texel(0.0f, 0.0f);

		const FSlateShaderResourceProxy* resource_proxy = draw_element_pay_load.get_resource_proxy();//todo:implement this function
		FSlateShaderResource* resource = nullptr;
		if (resource_proxy)
		{
			resource = resource_proxy->m_resource;
		
			//the width and height of the texture
			texture_width = resource_proxy->m_actual_size.x != 0 ? resource_proxy->m_actual_size.x : 1;
			texture_height = resource_proxy->m_actual_size.y != 0 ? resource_proxy->m_actual_size.y : 1;

			//texel offset

			const FBox2D& brush_uv = draw_element_pay_load.get_brush_uv_region();

			if (brush_uv.m_b_is_valid)
			{
				size_uv = brush_uv.get_size();
				start_uv = glm::vec2(brush_uv.m_min.x, brush_uv.m_min.y) + half_texel;
				end_uv = start_uv + size_uv;
			}
			else
			{
				size_uv = resource_proxy->m_size_uv;
				start_uv = resource_proxy->m_start_uv + half_texel;
				end_uv = start_uv + resource_proxy->m_size_uv;
			}
		}
		else
		{
			//todo:no texture, to implement FSlateShaderResourceProxy
			size_uv = glm::vec2(1.0f, 1.0f);
			half_texel = glm::vec2(0.0f, 0.0f);
		}
	
		glm::vec4 secondary_color(0.0f, 0.0f, 0.0f, 0.0f);

		//todo:implement FSlateRenderBatch
		FSlateRenderBatch& render_batch = create_render_batch(layer, resource, ESlateDrawPrimitive::TriangleList, in_draw_effects, draw_element);

		glm::vec2 tiling(0.0f, 0.0f);

		//todo:implement batch flags


		//todo:add shader parameters for extra rounded box parameters

		//the start index of these vertices in the index buffer
		const uint32_t index_start = 0;

		const FMargin& margin = draw_element_pay_load.get_brush_margin();

		const glm::vec2 top_right = glm::vec2(bottom_right.x, top_left.y);
		const glm::vec2 bottom_left = glm::vec2(top_left.x, bottom_right.y);

		if (draw_element_pay_load.get_brush_draw_type() != ESlateBrushDrawType::Image &&
			(margin.left != 0.0f || margin.top != 0.0f || margin.right != 0.0f || margin.bottom != 0.0f))
		{
			//create 9 quads for the box element based on the following diagram
			//     ___LeftMargin    ___RightMargin
			//    /                /
			//  +--+-------------+--+
			//  |  |c1           |c2| ___TopMargin
			//  +--o-------------o--+
			//  |  |             |  |
			//  |  |c3           |c4|
			//  +--o-------------o--+
			//  |  |             |  | ___BottomMargin
			//  +--+-------------+--+

			//determine the texture coordinates for each quad
			//these are not scaled
			float left_margin_u = (margin.left > 0.0f)
				? start_uv.x + margin.left * size_uv.x + half_texel.x : start_uv.x;

			float top_margin_v = (margin.top > 0.0f)
				? start_uv.y + margin.top * size_uv.y + half_texel.y : start_uv.y;

			float right_margin_u = (margin.right > 0.0f)
				? end_uv.x - margin.top * size_uv.x + half_texel.x : end_uv.x;

			float bottom_margin_v = (margin.bottom > 0.0f)
				? end_uv.y - margin.bottom * size_uv.y + half_texel.x : end_uv.y;

			//todo:interms of mirror to rejust uv

			//determine the margins for each quad
			float left_margin_x = texture_width * margin.left;
			float top_margin_y = texture_height * margin.top;
			float right_margin_x = local_size.x - texture_width * margin.right;
			float bottom_margin_y = local_size.y - texture_height * margin.bottom;

			glm::vec2 position = top_left;
			glm::vec2 end_pos = bottom_right;

			render_batch.add_vertex(FSlateVertex::Make<rounding>(render_transform, glm::vec2(position.x, position.y), local_size, draw_scale, glm::vec4(start_uv.x, start_uv.y, tiling.x, tiling.y), tint, secondary_color));//0
			render_batch.add_vertex(FSlateVertex::Make<rounding>(render_transform, glm::vec2(position.x, top_margin_y), local_size, draw_scale, glm::vec4(start_uv.x, top_margin_v, tiling.x, tiling.y), tint, secondary_color));//1
			render_batch.add_vertex(FSlateVertex::Make<rounding>(render_transform, glm::vec2(left_margin_x, position.y), local_size, draw_scale, glm::vec4(left_margin_u, start_uv.y, tiling.x, tiling.y), tint, secondary_color));//2 
			render_batch.add_vertex(FSlateVertex::Make<rounding>(render_transform, glm::vec2(left_margin_x, top_margin_y), local_size, draw_scale, glm::vec4(left_margin_u, top_margin_v, tiling.x, tiling.y), tint, secondary_color));//3
			render_batch.add_vertex(FSlateVertex::Make<rounding>(render_transform, glm::vec2(right_margin_x, position.y), local_size, draw_scale, glm::vec4(right_margin_u, start_uv.y, tiling.x, tiling.y), tint, secondary_color));//4
			render_batch.add_vertex(FSlateVertex::Make<rounding>(render_transform, glm::vec2(right_margin_x, top_margin_y), local_size, draw_scale, glm::vec4(right_margin_u, top_margin_y, tiling.x, tiling.y), tint, secondary_color));//5
			render_batch.add_vertex(FSlateVertex::Make<rounding>(render_transform, glm::vec2(end_pos.x, position.y), local_size, draw_scale, glm::vec4(end_uv.x, start_uv.y, tiling.x, tiling.y), tint, secondary_color));//6
			render_batch.add_vertex(FSlateVertex::Make<rounding>(render_transform, glm::vec2(end_pos.x, top_margin_y), local_size, draw_scale, glm::vec4(end_uv.x, top_margin_v, tiling.x, tiling.y), tint, secondary_color));//7

			render_batch.add_vertex(FSlateVertex::Make<rounding>(render_transform, glm::vec2(position.x, bottom_margin_y), local_size, draw_scale, glm::vec4(start_uv.x, bottom_margin_v, tiling.x, tiling.y), tint, secondary_color));//8
			render_batch.add_vertex(FSlateVertex::Make<rounding>(render_transform, glm::vec2(left_margin_x, bottom_margin_y), local_size, draw_scale, glm::vec4(left_margin_u, bottom_margin_v, tiling.x, tiling.y), tint, secondary_color));//9
			render_batch.add_vertex(FSlateVertex::Make<rounding>(render_transform, glm::vec2(right_margin_x, bottom_margin_y), local_size, draw_scale, glm::vec4(right_margin_u, bottom_margin_v, tiling.x, tiling.y), tint, secondary_color));//10
			render_batch.add_vertex(FSlateVertex::Make<rounding>(render_transform, glm::vec2(end_pos.x, bottom_margin_y), local_size, draw_scale, glm::vec4(end_uv.x, bottom_margin_v, tiling.x, tiling.y), tint, secondary_color));//11
			render_batch.add_vertex(FSlateVertex::Make<rounding>(render_transform, glm::vec2(position.x, end_pos.y), local_size, draw_scale, glm::vec4(start_uv.x, end_uv.y, tiling.x, tiling.y), tint, secondary_color));//12
			render_batch.add_vertex(FSlateVertex::Make<rounding>(render_transform, glm::vec2(left_margin_x, end_pos.y), local_size, draw_scale, glm::vec4(left_margin_u, end_uv.y, tiling.x, tiling.y), tint, secondary_color));//13
			render_batch.add_vertex(FSlateVertex::Make<rounding>(render_transform, glm::vec2(right_margin_x, end_pos.y), local_size, draw_scale, glm::vec4(right_margin_u, end_uv.y, tiling.x, tiling.y), tint, secondary_color));//14
			render_batch.add_vertex(FSlateVertex::Make<rounding>(render_transform, glm::vec2(end_pos.x, end_pos.y), local_size, draw_scale, glm::vec4(end_uv.x, end_uv.y, tiling.x, tiling.y), tint, secondary_color));//15

			//top
			render_batch.add_index(index_start + 0);
			render_batch.add_index(index_start + 1);
			render_batch.add_index(index_start + 2);
			render_batch.add_index(index_start + 2);
			render_batch.add_index(index_start + 1);
			render_batch.add_index(index_start + 3);

			render_batch.add_index(index_start + 2);
			render_batch.add_index(index_start + 3);
			render_batch.add_index(index_start + 4);
			render_batch.add_index(index_start + 4);
			render_batch.add_index(index_start + 3);
			render_batch.add_index(index_start + 5);

			render_batch.add_index(index_start + 4);
			render_batch.add_index(index_start + 5);
			render_batch.add_index(index_start + 6);
			render_batch.add_index(index_start + 6);
			render_batch.add_index(index_start + 5);
			render_batch.add_index(index_start + 7);

			//middle
			render_batch.add_index(index_start + 1);
			render_batch.add_index(index_start + 8);
			render_batch.add_index(index_start + 3);
			render_batch.add_index(index_start + 3);
			render_batch.add_index(index_start + 8);
			render_batch.add_index(index_start + 9);

			render_batch.add_index(index_start + 3);
			render_batch.add_index(index_start + 9);
			render_batch.add_index(index_start + 5);
			render_batch.add_index(index_start + 5);
			render_batch.add_index(index_start + 9);
			render_batch.add_index(index_start + 10);

			render_batch.add_index(index_start + 5);
			render_batch.add_index(index_start + 10);
			render_batch.add_index(index_start + 7);
			render_batch.add_index(index_start + 7);
			render_batch.add_index(index_start + 10);
			render_batch.add_index(index_start + 11);

			//bottom
			render_batch.add_index(index_start + 8);
			render_batch.add_index(index_start + 12);
			render_batch.add_index(index_start + 9);
			render_batch.add_index(index_start + 9);
			render_batch.add_index(index_start + 12);
			render_batch.add_index(index_start + 13);

			render_batch.add_index(index_start + 9);
			render_batch.add_index(index_start + 13);
			render_batch.add_index(index_start + 10);
			render_batch.add_index(index_start + 10);
			render_batch.add_index(index_start + 13);
			render_batch.add_index(index_start + 14);

			render_batch.add_index(index_start + 10);
			render_batch.add_index(index_start + 14);
			render_batch.add_index(index_start + 11);
			render_batch.add_index(index_start + 11);
			render_batch.add_index(index_start + 14);
			render_batch.add_index(index_start + 15);
		}
		else
		{
			//todo:implement this for image
			//todo:implement horizontal and vertical

			const glm::vec2 uv_min = start_uv;
			const glm::vec2 uv_max = end_uv;

			//horizontal
			start_uv.x = uv_max.x - (start_uv.x - uv_min.x);
			end_uv.x = uv_max.x - (end_uv.x - uv_min.x);

			//add four vertices to the list of verts to be added to the vertex buffer
			render_batch.add_vertex(FSlateVertex::Make<rounding>(render_transform, top_left, local_size, draw_scale, glm::vec4(start_uv.x, start_uv.y, tiling.x, tiling.y), tint, secondary_color));
			render_batch.add_vertex(FSlateVertex::Make<rounding>(render_transform, top_right, local_size, draw_scale, glm::vec4(end_uv.x, start_uv.y, tiling.x, tiling.y), tint, secondary_color));
			render_batch.add_vertex(FSlateVertex::Make<rounding>(render_transform, bottom_left, local_size, draw_scale, glm::vec4(start_uv.x, end_uv.y, tiling.x, tiling.y), tint, secondary_color));
			render_batch.add_vertex(FSlateVertex::Make<rounding>(render_transform, bottom_right, local_size, draw_scale, glm::vec4(end_uv.x, end_uv.y, tiling.x, tiling.y), tint, secondary_color));

			render_batch.add_index(index_start + 0);
			render_batch.add_index(index_start + 1);
			render_batch.add_index(index_start + 2);

			render_batch.add_index(index_start + 2);
			render_batch.add_index(index_start + 1);
			render_batch.add_index(index_start + 3);
		}
	}
}
