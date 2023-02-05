#include <PreCompileHeader.h>

#include "ShaderResourceManager.h"

#include "SlateCore/Styling/SlateBrush.h"

#include "SlateCore/Textures/SlateShaderResource.h"

namespace DoDo {
	FSlateShaderResourceManager::~FSlateShaderResourceManager()
	{
		for (auto it = m_resource_map.begin(); it != m_resource_map.end(); ++it)
		{
			delete it->second;//clear resource
		}
	}
	DoDoUtf8String FSlateShaderResourceManager::get_resource_path(const FSlateBrush& in_brush) const
	{
		//assume the brush name contains the whole path
		return in_brush.get_resource_name();
	}
	FSlateResourceHandle FSlateShaderResourceManager::get_resource_handle(const FSlateBrush& brush, glm::vec2 local_size, float draw_scale)
	{
		FSlateShaderResourceProxy* proxy = get_shader_resource(brush, local_size, draw_scale);//call specific derived function

		const FSlateResourceHandle& existing_handle = brush.m_resource_handle;

		if (proxy != existing_handle.get_resource_proxy())
		{
			FSlateResourceHandle new_handle;
			if (proxy)
			{
				if (!(proxy->m_handle_data))
				{
					proxy->m_handle_data = std::make_shared<FSlateSharedHandleData>(proxy);//todo:construct FSlateSharedHandleData
				}

				new_handle.m_data = proxy->m_handle_data;
			}

			return new_handle;
		}
		else
		{
			return existing_handle;
		}
	}
}