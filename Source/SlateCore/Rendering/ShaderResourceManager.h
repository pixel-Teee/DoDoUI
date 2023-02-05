#pragma once

#include "SlateCore/Textures/SlateTextureData.h"

#include "Core/String/DoDoString.h"

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
	class FSlateBrush;
	/*
	* base texture manager class used by a slate renderer to manager texture resources
	*/
	class FSlateShaderResourceManager
	{
	public:
		FSlateShaderResourceManager() {}

		virtual ~FSlateShaderResourceManager();

		DoDoUtf8String get_resource_path(const FSlateBrush& in_brush) const;

		//todo:add debug data

		/*mapping of names to texture pointers*/
		std::map<DoDoUtf8String, FSlateShaderResourceProxy*> m_resource_map;//FSlateShaderResourceManager holds the life time of FSlateShaderResourceProxy
	};
}