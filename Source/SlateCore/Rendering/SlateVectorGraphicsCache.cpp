#include "PreCompileHeader.h"

#include "SlateVectorGraphicsCache.h"

#include "SlateCore/Styling/SlateBrush.h"

#include "SlateCore/Rendering/SlateSVGRasterizer.h"

namespace DoDo {
	static FAtlasFlushParams SVG_Atlas_flush_params;

	FSlateVectorGraphicsCache::FSlateVectorGraphicsCache(std::shared_ptr<ISlateTextureAtlasFactory> in_atlas_factory)
		: FSlateFlushableAtlasCache(&SVG_Atlas_flush_params)
		, m_atlas_factory(in_atlas_factory)
		, m_b_flush_requested(false)
	{
	}
	FSlateShaderResourceProxy* FSlateVectorGraphicsCache::get_shader_resource(const FSlateBrush& brush, glm::vec2 local_size, float draw_scale)
	{
		FSlateShaderResourceProxy* proxy = nullptr;

		static double total_time = 0.0f;
		if (brush.get_image_type() == ESlateBrushImageType::Vector)
		{
			const FVectorCacheKey cache_key(brush.get_resource_name(), local_size, draw_scale);
			auto it = m_resource_map.find(cache_key);

			std::unique_ptr<FSlateShaderResourceProxy>* proxy_ptr = nullptr;

			if (it != m_resource_map.end())
			{
				proxy_ptr = &(it->second);
			}

			if (proxy_ptr)
			{
				proxy = proxy_ptr->get();
			}
			else
			{
				if (local_size.x > 0.0f && local_size.y > 0.0f)
				{
					m_pending_requests.emplace_back(brush.get_resource_name(), local_size, draw_scale);

					std::unique_ptr<FSlateShaderResourceProxy> new_proxy = std::make_unique<FSlateShaderResourceProxy>();

					proxy = new_proxy.get();

					//key is vector cache key
					m_resource_map.insert({ m_pending_requests.back().m_key, std::move(new_proxy) });
				}
			}
		}

		return proxy;
	}
	void FSlateVectorGraphicsCache::update_cache()
	{
		double time = 0;
		if (m_pending_requests.size() > 0)
		{
			const int32_t atlas_stride = 4;
			const int32_t atlas_size = 1024;
			const uint8_t padding = 1;

			for (size_t i = 0; i < m_pending_requests.size(); ++i)
			{
				FRasterRequest& current_request = m_pending_requests[i];

				current_request.m_pixel_data = FSlateSVGRasterizer::rasterize_svg_from_file(current_request.m_key.m_brush_name, current_request.m_key.m_pixel_size);
			}

			for (const FRasterRequest& request : m_pending_requests)
			{
				const std::vector<uint8_t>& pixel_data = request.m_pixel_data;
				FIntPoint pixel_size = request.m_key.m_pixel_size;

				if (pixel_data.size())
				{
					if (pixel_size.x > atlas_size || pixel_size.y > atlas_size)
					{
						std::unique_ptr<FSlateShaderResource> new_resource = m_atlas_factory->create_non_atlased_texture(pixel_size.x, pixel_size.y, pixel_data);

						//create proxy, put it in map
						auto it = m_resource_map.find(request.m_key);
					
						if (it != m_resource_map.end())
						{
							std::unique_ptr<FSlateShaderResourceProxy>& new_proxy = it->second;

							new_proxy->m_resource = new_resource.get();

							new_proxy->m_start_uv = glm::vec2(0, 0);
							new_proxy->m_size_uv = glm::vec2(1, 1);
							new_proxy->m_actual_size = FIntPoint(pixel_size.x, pixel_size.y);

							m_non_atlased_textures.push_back(std::move(new_resource));
						}
					}
					else
					{
						const FAtlasedTextureSlot* new_slot = nullptr;
						FSlateTextureAtlas* found_atlas = nullptr;
						for (std::unique_ptr<FSlateTextureAtlas>& atlas : m_atlases)
						{
							new_slot = atlas->add_texture(pixel_size.x, pixel_size.y, pixel_data);//add texture to new slot
							if (new_slot)
							{
								found_atlas = atlas.get();
							}
						}

						if (!new_slot)
						{
							const bool b_updates_after_initialization = true;
							std::unique_ptr<FSlateTextureAtlas> new_atlas = m_atlas_factory->create_texture_atlas(atlas_size, atlas_stride, ESlateTextureAtlasPaddingStyle::DilateBorder, b_updates_after_initialization);

							new_slot = new_atlas->add_texture(pixel_size.x, pixel_size.y, pixel_data);

							found_atlas = new_atlas.get();

							m_atlases.push_back(std::move(new_atlas));

							//todo:implement update flush counters
						}

						if (new_slot)
						{
							//create proxy, put it in map
							auto it = m_resource_map.find(request.m_key);
							if (it != m_resource_map.end())
							{
								std::unique_ptr<FSlateShaderResourceProxy>& new_proxy = it->second;

								new_proxy->m_resource = found_atlas->get_atlas_texture();

								//compute the sub-uvs for the location of this texture in the atlas, accounting for padding
								new_proxy->m_start_uv = glm::vec2((float)(new_slot->m_x + padding) / found_atlas->get_width(), (float)(new_slot->m_y + padding) / found_atlas->get_height());
								new_proxy->m_size_uv = glm::vec2((float)(new_slot->m_width - padding * 2) / found_atlas->get_width(), (float)(new_slot->m_height - padding * 2) / found_atlas->get_height());

								new_proxy->m_actual_size = FIntPoint(pixel_size.x, pixel_size.y);
							}
						}

						//todo:update flush counters
					}
				}
			}

			m_pending_requests.clear();

			for (std::unique_ptr<FSlateTextureAtlas>& atlas : m_atlases)
			{
				atlas->conditional_update_texture();
			}
		}
	}
}