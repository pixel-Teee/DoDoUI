#include <PreCompileHeader.h>

#include "ElementBatcher.h"

#include "DrawElements.h"
#include "SlateRenderBatch.h"
#include "glm/vec2.hpp"

namespace DoDo
{
	FSlateRenderBatch& FSlateBatchData::add_render_batch(int32_t in_layer, ESlateDrawPrimitive in_primitive_type,
		ESlateDrawEffect in_draw_effects)
	{
		return m_render_batches.emplace_back(in_layer, in_primitive_type, in_draw_effects, &m_uncached_source_batch_vertices, &m_uncached_source_batch_indices, m_uncached_source_batch_vertices.size(), m_uncached_source_batch_indices.size());
	}

	void FSlateBatchData::reset_data()
	{
		m_render_batches.clear();
	}

	void FSlateElementBatcher::add_elements(FSlateWindowElementList& element_list)
	{
		//todo:implement viewport size
		//glm::vec2 view_port_size = element_list.get_paint_window();
		m_batch_data = &element_list.get_batch_data();//get the batch data from element list, don't owner the life time of batch data

		add_elements_internal(element_list.get_uncached_draw_elements(), glm::vec2(1920.0f, 780.0f));
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

	FSlateRenderBatch& FSlateElementBatcher::create_render_batch(int32_t layer, ESlateDrawPrimitive primitive_type,
		ESlateDrawEffect draw_effects, const FSlateDrawElement& draw_element)
	{
		//render batch store the vertex and index data
		FSlateRenderBatch& new_batch = m_batch_data->add_render_batch(layer, primitive_type, draw_effects);

		return new_batch;
	}

	template <ESlateVertexRounding rounding>
	void FSlateElementBatcher::add_box_element(const FSlateDrawElement& draw_element)
	{
		//cast to specific type
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

		glm::vec2 half_texel;

		//todo:no texture, to implement FSlateShaderResourceProxy
		size_uv = glm::vec2(1.0f, 1.0f);
		half_texel = glm::vec2(0.0f, 0.0f);

		glm::vec4 secondary_color;

		//todo:implement FSlateRenderBatch
		FSlateRenderBatch& render_batch = create_render_batch(layer, ESlateDrawPrimitive::TriangleList, in_draw_effects, draw_element);

		glm::vec2 tiling;

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
		}
	}
}
