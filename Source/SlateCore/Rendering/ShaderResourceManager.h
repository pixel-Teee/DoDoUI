#pragma once

#include "SlateCore/Textures/SlateTextureData.h"

#include "Core/String/DoDoString.h"

#include "glm/glm.hpp"

#include "SlateCore/Textures/SlateShaderResource.h"

namespace DoDo {
	/*
	* represents a new texture that has been loaded but no resource created for it
	*/
	struct FNewTextureInfo
	{
		//friend class FSlateShaderResourceManager;
		/*raw data*/
		FSlateTextureDataPtr m_texture_data;

		/*whether or not the texture should be atlased*/
		bool m_b_should_atlas;

		/*whether or not the texture is in srgb space*/
		bool m_b_srgb;

		FNewTextureInfo()
			: m_b_should_atlas(true)
			, m_b_srgb(true)
		{}
	};
	class FSlateShaderResourceProxy;
	struct FSlateBrush;
	class FSlateResourceHandle;
	/*
	* base texture manager class used by a slate renderer to manager texture resources
	*/
	class FSlateShaderResourceManager
	{
	public:
		FSlateShaderResourceManager() {}

		virtual ~FSlateShaderResourceManager();

		virtual FSlateShaderResourceProxy* get_shader_resource(const FSlateBrush& in_brush, glm::vec2 local_size, float draw_scale) = 0;

		DoDoUtf8String get_resource_path(const FSlateBrush& in_brush) const;

		virtual FSlateResourceHandle get_resource_handle(const FSlateBrush& brush, glm::vec2 local_size, float draw_scale);

		//todo:add debug data

		/*mapping of names to texture pointers*/
		std::map<DoDoUtf8String, FSlateShaderResourceProxy*> m_resource_map;//FSlateShaderResourceManager holds the life time of FSlateShaderResourceProxy
	};
}