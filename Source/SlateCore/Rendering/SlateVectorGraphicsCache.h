#pragma once

#include "SlateCore/Textures/TextureAtlas.h"

#include "glm/glm.hpp"

#include "Core/Templates/TypeHash.h"

#include "Core/String/DoDoString.h"

#include "Core/Math/IntPoint.h"

#include <unordered_map>

namespace DoDo {
	struct FSlateBrush;
	class FSlateShaderResourceProxy;

	struct FVectorCacheKey
	{
		DoDoUtf8String m_brush_name;
		FIntPoint m_pixel_size;

		FVectorCacheKey(DoDoUtf8String in_brush_name, glm::vec2 local_size, float draw_scale)
			: m_brush_name(in_brush_name)
		{
			glm::vec2 temp_size = local_size * draw_scale;//trasfer to pixel size

			m_pixel_size = FIntPoint(temp_size.x, temp_size.y);

			std::hash<std::string> hasher;
			std::string s = std::string(m_brush_name.c_str());

			m_key_hash = Hash_Combine(hasher(s), Get_Type_Hash(m_pixel_size));
		}

		friend inline uint32_t Get_Type_Hash(const FVectorCacheKey& key)
		{
			return key.m_key_hash;
		}

		bool operator==(const FVectorCacheKey& other) const
		{
			return m_brush_name == other.m_brush_name && m_pixel_size == other.m_pixel_size;
		}

	private:
		uint32_t m_key_hash;
	};
}

template<>
struct std::hash<DoDo::FVectorCacheKey>
{
	std::size_t operator()(const DoDo::FVectorCacheKey& key) const
	{
		return Get_Type_Hash(key);
	}
};

namespace DoDo{

	class FSlateVectorGraphicsCache : public FSlateFlushableAtlasCache
	{
	public:
		FSlateVectorGraphicsCache(std::shared_ptr<ISlateTextureAtlasFactory> in_atlas_factory);

		FSlateShaderResourceProxy* get_shader_resource(const FSlateBrush& brush, glm::vec2 local_size, float draw_scale);

		void update_cache();
		
	private:

		struct FRasterRequest//a request to request raster svg image
		{
			FVectorCacheKey m_key;
			std::vector<uint8_t> m_pixel_data;

			FRasterRequest(DoDoUtf8String in_brush_name, glm::vec2 in_local_size, float in_draw_scale)
				: m_key(in_brush_name, in_local_size, in_draw_scale)
			{}
		};

		std::unordered_map<FVectorCacheKey, std::unique_ptr<FSlateShaderResourceProxy>> m_resource_map;
		
		std::shared_ptr<ISlateTextureAtlasFactory> m_atlas_factory;//use for create texture atlas

		std::vector<std::unique_ptr<FSlateShaderResource>> m_non_atlased_textures;//just some image size is larger than 1024, 1024

		std::vector<std::unique_ptr<FSlateTextureAtlas>> m_atlases;

		bool m_b_flush_requested;

		std::vector<FRasterRequest> m_pending_requests;
	};
}

