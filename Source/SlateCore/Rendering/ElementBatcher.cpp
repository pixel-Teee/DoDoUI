#include <PreCompileHeader.h>

#include "ElementBatcher.h"

#include "DrawElements.h"
#include "ShaderResourceManager.h"
#include "SlateRenderBatch.h"
#include "glm/vec2.hpp"
#include "SlateCore/Widgets/SWindow.h"

#include "SlateCore/Fonts/FontCache.h"
#include "SlateCore/Rendering/RenderingPolicy.h"

namespace DoDo
{
	FSlateRenderBatch& FSlateBatchData::add_render_batch(int32_t in_layer, const FShaderParams& in_shader_params, FSlateShaderResource* shader_resource, ESlateDrawPrimitive in_primitive_type, ESlateShader in_shader_type,
		ESlateDrawEffect in_draw_effects)
	{
		return m_render_batches.emplace_back(in_layer, in_shader_params, shader_resource, in_primitive_type, in_shader_type, in_draw_effects, &m_uncached_source_batch_vertices, &m_uncached_source_batch_indices, m_uncached_source_batch_vertices.size(), m_uncached_source_batch_indices.size());
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

	
	FSlateElementBatcher::FSlateElementBatcher(std::shared_ptr<FSlateRenderingPolicy> in_rendering_policy)
		: m_batch_data(nullptr)
		, m_rendering_policy(in_rendering_policy.get())
		, m_b_srgb_vertex_color(false)//note:implement FSlateRenderingPolicy is vertex color in linear space function
	{
	}

	FSlateElementBatcher::~FSlateElementBatcher()
	{
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
				break;
			}
			case EElementType::ET_Text:
			{
				add_text_element<ESlateVertexRounding::Disabled>(draw_element);
				break;
			}
			case EElementType::ET_ShapedText:
			{
				add_shaped_text_element<ESlateVertexRounding::Disabled>(draw_element);
				break;
			}
			case EElementType::ET_Gradient:
			{
				add_gradient_element<ESlateVertexRounding::Disabled>(draw_element);
				break;
			}
			}
		}
	}

	FSlateRenderBatch& FSlateElementBatcher::create_render_batch(int32_t layer, const FShaderParams& shader_params, FSlateShaderResource* shader_resource, ESlateDrawPrimitive primitive_type, ESlateShader in_shader_type,
		ESlateDrawEffect draw_effects, const FSlateDrawElement& draw_element)
	{
		//render batch store the vertex and index data
		FSlateRenderBatch& new_batch = m_batch_data->add_render_batch(layer, shader_params, shader_resource, primitive_type, in_shader_type, draw_effects);

		return new_batch;
	}

	template <ESlateVertexRounding rounding>
	void FSlateElementBatcher::add_box_element(const FSlateDrawElement& draw_element)
	{
		//cast to specific type
		//note:box pay load is just some draw related information(don't contain the information about the geometry)
		const FSlateBoxPayload& draw_element_pay_load = draw_element.get_data_pay_load<FSlateBoxPayload>();

		//todo:implement from linear color to srgb color
		const FColor tint = pack_vertex_color(draw_element_pay_load.get_tint());

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

		const ESlateBrushTileType::Type tiling_rule = draw_element_pay_load.get_brush_tiling();
		const bool b_tile_horizontal = (tiling_rule == ESlateBrushTileType::Both || tiling_rule == ESlateBrushTileType::Horizontal);
		const bool b_tile_vertical = (tiling_rule == ESlateBrushTileType::Both || tiling_rule == ESlateBrushTileType::Vertical);

		//todo:implement ESlateBrushMirrorType

		//todo:add shader parameters for extra rounded box parameters
		ESlateShader shader_type = ESlateShader::Default;
		FShaderParams shader_params;
		FColor secondary_color;
		if (draw_element.get_element_type() == EElementType::ET_RoundedBox)
		{
			shader_type = ESlateShader::RoundedBox;

			const FSlateRoundedBoxPayload& rounded_pay_load = draw_element.get_data_pay_load<FSlateRoundedBoxPayload>();

			shader_params.m_pixel_params = glm::vec4(0.0f, rounded_pay_load.get_outline_weight(), local_size.x, local_size.y);//radius weight

			shader_params.m_pixel_params2 = rounded_pay_load.get_radius();

			secondary_color = pack_vertex_color(rounded_pay_load.m_outline_color);
		}	

		//todo:implement FSlateRenderBatch
		FSlateRenderBatch& render_batch = create_render_batch(layer, shader_params, resource, ESlateDrawPrimitive::TriangleList, shader_type, in_draw_effects, draw_element);

		float horizontal_tiling = b_tile_horizontal ? local_size.x / texture_width : 1.0f;
		float vertical_tiling = b_tile_vertical ? local_size.y / texture_height : 1.0f;

		glm::vec2 tiling(horizontal_tiling, vertical_tiling);

		//todo:implement batch flags

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
				? end_uv.x - margin.right * size_uv.x + half_texel.x : end_uv.x;

			float bottom_margin_v = (margin.bottom > 0.0f)
				? end_uv.y - margin.bottom * size_uv.y + half_texel.y : end_uv.y;

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
			render_batch.add_vertex(FSlateVertex::Make<rounding>(render_transform, glm::vec2(right_margin_x, top_margin_y), local_size, draw_scale, glm::vec4(right_margin_u, top_margin_v, tiling.x, tiling.y), tint, secondary_color));//5
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
			//start_uv.x = uv_max.x - (start_uv.x - uv_min.x);
			//end_uv.x = uv_max.x - (end_uv.x - uv_min.x);

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

	template <ESlateVertexRounding rounding>
	void FSlateElementBatcher::add_text_element(const FSlateDrawElement& draw_element)
	{
		const FSlateTextPayload& draw_element_pay_load = draw_element.get_data_pay_load<FSlateTextPayload>();

		FLinearColor base_tint = draw_element_pay_load.get_tint();

		//todo:implement FFontOutlineSettings

		int32_t len = draw_element_pay_load.get_text_length();

		ESlateDrawEffect in_draw_effects = draw_element.get_draw_effects();

		const int32_t layer = draw_element.get_layer();

		//extract the layout transform from the draw element
		FSlateLayoutTransform layout_transform(draw_element.get_scale(), draw_element.get_position());

		//we don't just scale up fonts, we draw them in local space pre-scaled so we don't get scaling artifacts
		//so we need to pull the layout scale out of the layout and render transform so we can apply them
		//in local space with pre-scaled fonts
		const float font_scale = layout_transform.get_scale();
		FSlateLayoutTransform inverse_layout_transform = inverse(concatenate(inverse(font_scale), layout_transform));
		const FSlateRenderTransform render_transform = concatenate(inverse(font_scale), draw_element.get_render_transform());
		//concatenate(inverse(font_scale), layout_transform);

		//todo:implement font cache
		FSlateFontCache& font_cache = *(m_rendering_policy->get_font_cache());
		//todo:get resource manager
		FSlateShaderResourceManager& resource_manager = *(m_rendering_policy->get_resource_manager());//note:texture manager

		auto build_font_geometry = [&](const FFontOutlineSettings& in_outline_settings, const FColor& in_tint, int32_t in_layer)
		{
			FCharacterList& character_list = font_cache.get_character_list(draw_element_pay_load.get_font_info(), font_scale, in_outline_settings);

			float max_height = character_list.get_max_height();

			//todo:implement get max height

			uint32_t font_texture_index = 0;
			FSlateShaderResource* font_atlas_texture = nullptr;
			FSlateShaderResource* font_shader_resource = nullptr;
			FColor font_tint = in_tint;

			FSlateRenderBatch* render_batch = nullptr;
			FSlateVertexArray* batch_vertices = nullptr;
			FSlateIndexArray* batch_indices = nullptr;

			uint32_t vertex_offset = 0;
			uint32_t index_offset = 0;

			float inv_texture_size_x = 0;
			float inv_texture_size_y = 0;

			float line_x = 0;

			FCharacterEntry previous_entry;

			int32_t kerning = 0;

			glm::vec2 top_left(0.0f, 0.0f);

			const float pos_x = top_left.x;
			float pos_y = top_left.y;

			line_x = pos_x;

			//todo:check outline enable

			uint32_t num_chars = len;

			uint32_t num_lines = 1;

			for (uint32_t char_index = 0; char_index < num_chars; ++char_index)
			{
				DoDoUtf8String current_char = draw_element_pay_load.get_text().utf8_at(char_index);

				const bool is_new_line = (current_char.utf8_at(0) == DoDoUtf8String("\n"));

				if (is_new_line)
				{
					pos_y += max_height;

					line_x = pos_x;

					++num_lines;
				}
				else
				{
					const FCharacterEntry& entry = character_list.get_character(current_char, draw_element_pay_load.get_font_info().m_font_fallback);

					if (entry.m_valid && (font_atlas_texture == nullptr || entry.m_texture_index != font_texture_index))
					{
						//font has a new texture for this glyph, refresh the batch we use and the index we are currently using

						font_texture_index = entry.m_texture_index;

						ISlateFontTexture* slate_font_texture = font_cache.get_font_texture(font_texture_index);

						font_atlas_texture = slate_font_texture->get_slate_texture();//get real gpu texture, font texture holds this 

						font_shader_resource = font_atlas_texture;//note:unreal will have material

						const bool b_is_gray_scale = slate_font_texture->is_gray_scale();

						font_tint = b_is_gray_scale ? in_tint : FColor::White;//white

						render_batch = &create_render_batch(in_layer, FShaderParams(), font_shader_resource, ESlateDrawPrimitive::TriangleList, b_is_gray_scale ? ESlateShader::GrayScaleFont : ESlateShader::ColorFont, in_draw_effects, draw_element);

						//reserve memory for the glyphs, this isn't perfect as the text could contain spaces and we might not render the rest of the text in this batch but its
						//better than resizing constantly
						const int32_t glyphs_left = num_chars - char_index;

						render_batch->reserve_vertices(glyphs_left * 4);
						render_batch->reserve_indices(glyphs_left * 6);

						inv_texture_size_x = 1.0f / font_atlas_texture->get_width();
						inv_texture_size_y = 1.0f / font_atlas_texture->get_height();
					}

					const bool b_is_white_space = !entry.m_valid || current_char.utf8_at(0) == 0;//todo:check

					if (!b_is_white_space && previous_entry.m_valid)
					{
						//kerning = character_list.get_kerning();
						//todo:implement get kerning
					}
					else
					{
						kerning = 0;
					}

					line_x += kerning;
					previous_entry = entry;

					if (!b_is_white_space)
					{
						const float inv_bit_map_render_scale = 1.0f / entry.m_bitmap_render_scale;

						const float x = line_x + entry.m_horizontal_offset;
						//note posx, posy is the upper left corner of the bounding box representing the string, this computes the y position of the baseline where text will sit

						//todo:implement max_height
						const float y = pos_y - entry.m_vertical_offset + ((max_height + entry.m_global_descendar) * inv_bit_map_render_scale);
						const float u = entry.m_start_u * inv_texture_size_x;
						const float v = entry.m_start_v * inv_texture_size_y;
						const float size_x = entry.m_u_size * entry.m_bitmap_render_scale;
						const float size_y = entry.m_v_size * entry.m_bitmap_render_scale;
						const float size_u = entry.m_u_size * inv_texture_size_x;
						const float size_v = entry.m_v_size * inv_texture_size_y;

						glm::vec2 upper_left(x, y);
						glm::vec2 upper_right(x + size_x, y);
						glm::vec2 lower_left(x, y + size_y);
						glm::vec2 lower_right(x + size_x, y + size_y);

						//the start index of these vertices in the index buffer
						const uint32_t index_start = render_batch->get_num_vertices();

						float ut = 0.0f, vt = 0.0f, ut_max = 0.0f, vt_max = 0.0f;

						//add four vertices to the list of verts to be added to the vertex buffer
						render_batch->add_vertex(FSlateVertex::Make<rounding>(render_transform, upper_left, glm::vec4(u, v, ut, vt), glm::vec2(0.0f, 0.0f), font_tint));
						render_batch->add_vertex(FSlateVertex::Make<rounding>(render_transform, glm::vec2(lower_right.x, upper_left.y), glm::vec4(u + size_u, v, ut_max, vt), glm::vec2(1.0f, 0.0f), font_tint));
						render_batch->add_vertex(FSlateVertex::Make<rounding>(render_transform, glm::vec2(upper_left.x, lower_right.y), glm::vec4(u, v + size_v, ut, vt_max), glm::vec2(0.0f, 1.0f), font_tint));
						render_batch->add_vertex(FSlateVertex::Make<rounding>(render_transform, lower_right, glm::vec4(u + size_u, v + size_v, ut_max, vt_max), glm::vec2(1.0f, 1.0f), font_tint));

						render_batch->add_index(index_start + 0);
						render_batch->add_index(index_start + 1);
						render_batch->add_index(index_start + 2);
						render_batch->add_index(index_start + 1);
						render_batch->add_index(index_start + 3);
						render_batch->add_index(index_start + 2);
					}

					line_x += entry.m_x_advance;
				}
			}
		};

		//todo:implement outline

		//no outline, draw normally
		build_font_geometry(FFontOutlineSettings::NoOutline, pack_vertex_color(base_tint), layer);
	}

	template<ESlateVertexRounding rounding>
	void FSlateElementBatcher::build_shaped_text_sequence(const FShapedTextBuildContext& context)
	{
		const FShapedGlyphSequence* glyph_sequence_to_render = context.m_shaped_glyph_sequence;

		FSlateShaderResourceManager& resource_manager = *m_rendering_policy->get_resource_manager();

		float inv_texture_size_y = 0;
		float inv_texture_size_x = 0;

		FSlateRenderBatch* render_batch = nullptr;

		int32_t font_texture_index = -1;
		FSlateShaderResource* font_atlas_texture = nullptr;
		FSlateShaderResource* font_shader_resource = nullptr;

		float line_x = context.m_start_line_x;
		float line_y = context.m_start_line_y;

		FSlateRenderTransform render_transform = *context.m_render_transform;

		FColor tint = FColor::White;

		ETextOverflowDirection overflow_direction = context.m_overflow_direction;

		float ellipsis_line_x = 0;
		float ellipsis_line_y = 0;
		bool b_need_ellipsis = false;
		bool b_character_was_clipped = false;

		//for left to right overflow direction - sum of total white space we're currently advancing through
		//once a non-whitespace glyph is detected this will return to 0
		float previous_white_space_advance = 0;

		int32_t num_glyphs = glyph_sequence_to_render->get_glyphs_to_render().size();

		const std::vector<FShapedGlyphEntry>& glyphs_to_render = glyph_sequence_to_render->get_glyphs_to_render();

		for (int32_t glyph_index = 0; glyph_index < num_glyphs; ++glyph_index)
		{
			const FShapedGlyphEntry& glyph_to_render = glyphs_to_render[glyph_index];

			const float bitmap_render_scale = glyph_to_render.get_bit_map_render_scale();
			const float inv_bitmap_render_scale = 1.0f / bitmap_render_scale;

			float x = 0;
			float size_x = 0;
			float y = 0;
			float u = 0;
			float v = 0;
			float size_y = 0;
			float size_u = 0;
			float size_v = 0;

			bool b_can_render_glyph = glyph_to_render.m_b_is_visible;
			if (b_can_render_glyph)
			{
				//get sizing and atlas info
				const FShapedGlyphFontAtlasData glyph_atlas_data = context.m_font_cache->get_shaped_glyph_font_atlas_data(glyph_to_render, *context.m_outline_settings);

				if (glyph_atlas_data.m_valid && (!context.m_b_enable_outline || glyph_atlas_data.m_supports_outline))
				{
					x = line_x + glyph_atlas_data.m_horizontal_offset + glyph_to_render.m_x_offset;
					//note posx, posy is the upper left corner of the bounding box representing the string, this computes the y position of the baseline where text will sit

					//todo:add culling logic

					if (font_atlas_texture == nullptr || glyph_atlas_data.m_texture_index != font_texture_index)
					{
						//font has a new texture for this glyph, refresh the batch we use and the index we are currently using
						font_texture_index = glyph_atlas_data.m_texture_index;

						ISlateFontTexture* slate_font_texture = context.m_font_cache->get_font_texture(font_texture_index);

						font_atlas_texture = slate_font_texture->get_slate_texture();

						font_shader_resource = resource_manager.get_font_shader_resource(font_texture_index, font_atlas_texture);

						const bool b_is_gray_scale = slate_font_texture->is_gray_scale();
						tint = b_is_gray_scale ? context.m_font_tint : FColor::White;

						render_batch = &create_render_batch(context.m_layer_id, FShaderParams(), font_shader_resource, ESlateDrawPrimitive::TriangleList,
							b_is_gray_scale ? ESlateShader::GrayScaleFont : ESlateShader::ColorFont, context.m_draw_element->get_draw_effects(), *context.m_draw_element);

						//reserve memory for the glyphs, this isn't perfect as the text could contain spaces and we might not render the rest of the text in this batch but its better than resizing
						//constantly
						const int32_t glyphs_left = num_glyphs - glyph_index;
						render_batch->reserve_vertices(glyphs_left * 4);
						render_batch->reserve_indices(glyphs_left * 6);

						inv_texture_size_x = 1.0f / font_atlas_texture->get_width();
						inv_texture_size_y = 1.0f / font_atlas_texture->get_height();
					}

					y = line_y - glyph_atlas_data.m_vertical_offset + glyph_to_render.m_y_offset + ((context.m_max_height + context.m_text_base_line) * inv_bitmap_render_scale);
					u = glyph_atlas_data.m_start_u * inv_texture_size_x;
					v = glyph_atlas_data.m_start_v * inv_texture_size_y;
					size_x = glyph_atlas_data.m_u_size * bitmap_render_scale;
					size_y = glyph_atlas_data.m_v_size * bitmap_render_scale;
					size_u = glyph_atlas_data.m_u_size * inv_texture_size_x;
					size_v = glyph_atlas_data.m_v_size * inv_texture_size_y;
				}
				else
				{
					b_can_render_glyph = false;
				}
			}
			else
			{
				x = line_x;
				size_x = glyph_to_render.m_x_advance;
			}

			//overflow detection
			//first figure out the size of the glyph, if the glyph contains multiple characters we have to measure all of them and if clicked
			//this is common in complex languages with lots of diacritics
			float overflow_test_width = size_x;
			if (overflow_direction != ETextOverflowDirection::NoOverflow && (glyph_to_render.m_num_grapheme_clusters_in_glyph > 1 || glyph_to_render.m_num_characters_in_glyph > 1))
			{
				const int32_t start_index = glyph_index;
				int32_t end_index = glyph_index;
				int32_t next_index = glyph_index + 1;
				const int32_t source_index = glyph_to_render.m_source_index;

				while (next_index < num_glyphs && glyphs_to_render[next_index].m_source_index == source_index)
				{
					++end_index;
					++next_index;
				}

				if (start_index < end_index)
				{
					overflow_test_width = glyph_sequence_to_render->get_measured_width(start_index, end_index).value_or(size_x);
				}
			}

			//left to right overflow - if the current pen position + the ellipsis cannot fit, we have reached the end of the possible area for drawing this text
			if (overflow_direction == ETextOverflowDirection::LeftToRight)
			{
				//if we are on the last glyph don't bother checking if the ellipsis can fit, if the last glyph can fit here is no need for ellipsis
				float overflow_sequence_needed_size = glyph_index < num_glyphs - 1 ? context.m_overflow_glyph_sequence->get_measured_width() : 0;

				if (x + overflow_test_width + overflow_sequence_needed_size >= context.m_local_clip_bounding_box_left)
				{
					b_need_ellipsis = true;

					//todo:add more logic

					break;
				}
			}
			else if (overflow_direction == ETextOverflowDirection::RightToLeft)
			{
				bool b_clipped = false;

				//todo:add more logic

				b_can_render_glyph = !b_clipped;
			}

			if (b_can_render_glyph && render_batch)
			{
				const glm::vec2 upper_left(x, y);
				const glm::vec2 upper_right(x + size_x, y);
				const glm::vec2 lower_left(x, y + size_y);
				const glm::vec2 lower_right(x + size_x, y + size_y);

				//the start index of these vertices in the index buffer
				const uint32_t index_start = render_batch->get_num_vertices();

				float ut = 0.0f, vt = 0.0f, ut_max = 0.0f, vt_max = 0.0f;
				
				//add four vertices to the list of verts to be added to the vertex buffer
				render_batch->add_vertex(FSlateVertex::Make<rounding>(render_transform, glm::vec2(upper_left), glm::vec4(u, v, ut, vt), glm::vec2(0.0f, 0.0f), tint));
				render_batch->add_vertex(FSlateVertex::Make<rounding>(render_transform, glm::vec2(lower_right.x, upper_left.y), glm::vec4(u + size_u, v, ut_max, vt), glm::vec2(1.0f, 0.0f), tint));
				render_batch->add_vertex(FSlateVertex::Make<rounding>(render_transform, glm::vec2(upper_left.x, lower_right.y), glm::vec4(u, v + size_v, ut, vt_max), glm::vec2(0.0f, 1.0f), tint));
				render_batch->add_vertex(FSlateVertex::Make<rounding>(render_transform, glm::vec2(lower_right), glm::vec4(u + size_u, v + size_v, ut_max, vt_max), glm::vec2(1.0f, 1.0f), tint));

				render_batch->add_index(index_start + 0);
				render_batch->add_index(index_start + 1);
				render_batch->add_index(index_start + 2);
				render_batch->add_index(index_start + 1);
				render_batch->add_index(index_start + 3);
				render_batch->add_index(index_start + 2);

				//reset whitespace advance to 0, this is a visible character
				previous_white_space_advance = 0;
			}
			else if (!glyph_to_render.m_b_is_visible)
			{
				//how much whitespace we are currently walking through
				previous_white_space_advance += glyph_to_render.m_x_advance;
			}

			line_x += glyph_to_render.m_x_advance;
			line_y += glyph_to_render.m_y_advance;

		}

		//todo::add ellipsis
	}

	template<ESlateVertexRounding rounding>
	void FSlateElementBatcher::add_shaped_text_element(const FSlateDrawElement& draw_element)
	{
		const FSlateShapedTextPayload& draw_element_pay_load = draw_element.get_data_pay_load<FSlateShapedTextPayload>();

		const FShapedGlyphSequence* shaped_glyph_sequence = draw_element_pay_load.get_shaped_glyph_sequence().get();

		const FShapedGlyphSequence* overflow_glyph_sequence = draw_element_pay_load.m_overflow_args.m_overflow_text_ptr.get();

		const FFontOutlineSettings& outline_settings = shaped_glyph_sequence->get_font_outline_settings();

		const FColor base_tint = pack_vertex_color(draw_element_pay_load.get_tint());

		FSlateFontCache& font_cache = *m_rendering_policy->get_font_cache();

		const int16_t text_base_line = shaped_glyph_sequence->get_text_base_line();

		const uint16_t max_height = shaped_glyph_sequence->get_max_text_height();

		FShapedTextBuildContext build_context;

		build_context.m_draw_element = &draw_element;
		build_context.m_font_cache = &font_cache;
		build_context.m_shaped_glyph_sequence = shaped_glyph_sequence;
		build_context.m_overflow_glyph_sequence = overflow_glyph_sequence;
		build_context.m_text_base_line = text_base_line;
		build_context.m_max_height = max_height;

		if (max_height == 0)
		{
			//if the max text height is 0, we'll create NaN's further in the code, so avoid drawing text if this happens
			return;
		}

		const int32_t layer = draw_element.get_layer();

		//extract the layout transform from the draw element
		FSlateLayoutTransform layout_transform(draw_element.get_scale(), draw_element.get_position());

		//we don't just scale up fonts, we draw them in local space pre-scaled so we don't get scaling artifacts
		//so we need to pull the layout scale out of the layout and render transform so we can apply them
		//in local space with pre-scaled fonts
		const float font_scale = layout_transform.get_scale();

		const FSlateRenderTransform render_transform = concatenate(inverse(font_scale), draw_element.get_render_transform());

		build_context.m_render_transform = &render_transform;

		bool b_outline_font = outline_settings.m_outline_size > 0.0f;
		const int32_t outline_size = outline_settings.m_outline_size;

		auto build_geometry = [&](const FFontOutlineSettings& in_outline_settings, const FColor& in_tint, int32_t in_layer, float in_horizontal_offset)
		{
			glm::vec2 top_left(0, 0);

			const float pos_x = top_left.x + in_horizontal_offset;
			float pos_y = top_left.y;

			build_context.m_outline_settings = &in_outline_settings;
			build_context.m_start_line_x = pos_x;
			build_context.m_start_line_y = pos_y;
			build_context.m_layer_id = in_layer;
			build_context.m_font_tint = in_tint;

			build_context.m_b_enable_outline = in_outline_settings.m_outline_size > 0.0f;

			//optimize by culling
			build_context.m_b_enable_culling = false;
			build_context.m_b_force_ellipsis = draw_element_pay_load.m_overflow_args.m_b_force_ellipsis_due_to_clipped_line;
			build_context.m_overflow_direction = draw_element_pay_load.m_overflow_args.m_overflow_direction;

			if (shaped_glyph_sequence->get_glyphs_to_render().size() > 200 || (overflow_glyph_sequence && build_context.m_overflow_direction != ETextOverflowDirection::NoOverflow))
			{
				//todo:add clip logic

				//overflow not supported on non-identity transforms (except for 90-degree rotations)
				build_context.m_overflow_direction = ETextOverflowDirection::NoOverflow;
			}

			build_shaped_text_sequence<rounding>(build_context);
		};

		if (b_outline_font)
		{
			//build geometry for the outline
			build_geometry(outline_settings, pack_vertex_color(draw_element_pay_load.get_outline_tint()), layer, 0.0f);

			//the fill area was measured without an outline so it must be shifted by the scaled outline size
			const float horizontal_offset = outline_size * font_scale;

			//build geometry for the base font which is always rendered on top of the outline
			build_geometry(FFontOutlineSettings::NoOutline, base_tint, layer + 1, horizontal_offset);
		}
		else
		{
			//no outline
			build_geometry(FFontOutlineSettings::NoOutline, base_tint, layer, 0.0f);
		}
	}

	template<ESlateVertexRounding Rounding>
	void FSlateElementBatcher::add_gradient_element(const FSlateDrawElement& draw_element)
	{
		const FSlateRenderTransform& render_transform = draw_element.get_render_transform();

		const glm::vec2 local_size = draw_element.get_local_size();

		const FSlateGradientPayload& in_pay_load = draw_element.get_data_pay_load<FSlateGradientPayload>();

		const ESlateDrawEffect in_draw_effects = draw_element.get_draw_effects();

		const int32_t layer = draw_element.get_layer();

		const float draw_scale = draw_element.get_scale();

		//todo:implement FShaderParams
		FShaderParams shader_params;

		ESlateShader shader_type = ESlateShader::Default;
		if (in_pay_load.m_corner_radius != glm::vec4(0.0f))
		{
			shader_type = ESlateShader::RoundedBox;
			shader_params.m_pixel_params = glm::vec4(0.0f, 0.0f, local_size.x, local_size.y);
			shader_params.m_pixel_params2 = in_pay_load.m_corner_radius;
		}

		FSlateRenderBatch& render_batch =
			create_render_batch(
				layer,
				shader_params,
				nullptr,
				ESlateDrawPrimitive::TriangleList,
				shader_type,
				in_draw_effects,
				draw_element
			);

		//determine the four corners of the quad containing the gradient
		glm::vec2 top_left = glm::vec2(0.0f);
		glm::vec2 top_right = glm::vec2(local_size.x, 0.0f);
		glm::vec2 bot_left = glm::vec2(0.0f, local_size.y);
		glm::vec2 bot_right = glm::vec2(local_size.x, local_size.y);

		//copy the gradient stops, we may need to add more
		std::vector<FSlateGradientStop> gradient_stops = in_pay_load.m_gradient_stops;

		const FSlateGradientStop& first_stop = in_pay_load.m_gradient_stops[0];
		const FSlateGradientStop& last_stop = in_pay_load.m_gradient_stops[in_pay_load.m_gradient_stops.size() - 1];

		//determine if the first and last stops are not at the start and end of the quad
		//if they are not add a gradient stop with the same color as the first and/or last stop
		if(in_pay_load.m_gradient_type == Orient_Vertical)
		{
			if(0.0f < first_stop.m_position.x)
			{
				//the first stop is after the left side of the quad, add a stop at the left side of the quad using the same color as the first stop
				gradient_stops.insert(gradient_stops.begin(), FSlateGradientStop(glm::vec2(0.0f), first_stop.m_color));
			}

			if(local_size.x > last_stop.m_position.x)
			{
				//the last stop is before the right side of the quad, add a stop at the right side of the quad using the same color as the last stop
				gradient_stops.push_back(FSlateGradientStop(local_size, last_stop.m_color));
			}
		}
		else
		{
			if(0.0f < first_stop.m_position.y)
			{
				//the first stop is after the top side of the quad, add a stop at the top side of the quad using the same color as the first stop
				gradient_stops.insert(gradient_stops.begin(), FSlateGradientStop(glm::vec2(0.0f), first_stop.m_color));
			}

			if(local_size.y > last_stop.m_position.y)
			{
				//the last stop is before the bottom side of the quad, add a stop at the bottom side of the quad using the same color as the last stop
				gradient_stops.push_back(FSlateGradientStop(local_size, last_stop.m_color));
			}
		}

		//add a pair of vertices for each gradient stop, connecting them to the previous stop if necessary
		//assumes gradient stops are sorted by position left to right or top to bottom
		for(int32_t stop_index = 0; stop_index < gradient_stops.size(); ++stop_index)
		{
			const uint32_t index_start = render_batch.get_num_vertices();

			const FSlateGradientStop& cur_stop = gradient_stops[stop_index];

			//the start vertex at this stop
			glm::vec2 start_pt;
			//the end vertex at this stop
			glm::vec2 end_pt;

			glm::vec2 start_uv;
			glm::vec2 end_uv;

			if(in_pay_load.m_gradient_type == Orient_Vertical)
			{
				//gradient stop is vertical so gradients to left to right
				start_pt = top_left;
				end_pt = bot_left;

				//gradient stops are interpreted in local space
				start_pt.x += cur_stop.m_position.x;
				end_pt.x += cur_stop.m_position.x;

				start_uv.x = start_pt.x / top_right.x;
				start_uv.y = 0;

				end_uv.x = end_pt.x / top_right.x;
				end_uv.y = 1;
			}
			else
			{
				//gradient stop is horizontal so gradients to top to bottom
				start_pt = top_left;
				end_pt = top_right;

				//gradient stops are interpreted in local space
				start_pt.y += cur_stop.m_position.y;
				end_pt.y += cur_stop.m_position.y;

				start_uv.x = 0;
				start_uv.y = start_pt.y / bot_left.y;

				end_uv.x = 1;
				end_uv.y = start_pt.y / bot_left.y;
			}

			if(stop_index == 0)
			{
				//first stop does not have a full quad yet so do not create indices
				//todo:fix me
				render_batch.add_vertex(FSlateVertex::Make<Rounding>(render_transform, start_pt, local_size, draw_scale, glm::vec4(start_uv.x, start_uv.y, 0, 0), pack_vertex_color(cur_stop.m_color), FColor::Transparent));
				render_batch.add_vertex(FSlateVertex::Make<Rounding>(render_transform, end_pt, local_size, draw_scale, glm::vec4(end_uv.x, end_uv.y, 0, 0), pack_vertex_color(cur_stop.m_color), FColor::Transparent));
			}
			else
			{
				//all stops after the first have indices and generate quads
				render_batch.add_vertex(FSlateVertex::Make<Rounding>(render_transform, start_pt, local_size, draw_scale, glm::vec4(start_uv.x, start_uv.y, 0, 0), pack_vertex_color(cur_stop.m_color), FColor::Transparent));
				render_batch.add_vertex(FSlateVertex::Make<Rounding>(render_transform, end_pt, local_size, draw_scale, glm::vec4(end_uv.x, end_uv.y, 0, 0), pack_vertex_color(cur_stop.m_color), FColor::Transparent));

				//connect the indices to the previous vertices

				render_batch.add_index(index_start + 0);
				render_batch.add_index(index_start - 1);
				render_batch.add_index(index_start - 2);

				render_batch.add_index(index_start + 1);
				render_batch.add_index(index_start - 1);
				render_batch.add_index(index_start + 0);
			}
		}
	}
}
