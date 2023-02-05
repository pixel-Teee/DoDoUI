#include <PreCompileHeader.h>

#include "ShaderResourceManager.h"

#include "SlateCore/Styling/SlateBrush.h"

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
}