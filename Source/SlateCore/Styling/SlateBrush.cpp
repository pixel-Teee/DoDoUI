#include <PreCompileHeader.h>

#include "SlateBrush.h"

#include "Application/Application.h"

#include "Renderer/Renderer.h"

namespace DoDo
{
	FSlateBrush::FSlateBrush()
	{
	}

	FSlateBrush::~FSlateBrush()
	{
	}
	void FSlateBrush::update_rendering_resource(glm::vec2 local_size, float draw_scale) const
	{
		if (m_draw_as != ESlateBrushDrawType::NoDrawType && (m_resource_name != "" || m_resource_object != nullptr))
		{
			if (!m_resource_handle.is_valid() || m_image_type == ESlateBrushImageType::Vector)
			{
				m_resource_handle = Application::get().get_renderer()->get_resource_handle(*this, local_size, draw_scale);
			}
		}
	}
	//todo:called by Slate Rounded Box Brush
	FSlateBrush::FSlateBrush(ESlateBrushDrawType::Type in_draw_type, 
		const DoDoUtf8String in_source_name, const FMargin& in_margin, 
		ESlateBrushTileType::Type in_tiling, ESlateBrushImageType::Type in_image_type, 
		const glm::vec2& in_image_size, const FLinearColor& in_tint, void* in_object_resource, bool b_in_dynamically_loaded)
		: m_image_size(in_image_size)
		, m_margin(in_margin)
		, m_tint_color(in_tint)
		, m_resource_object((Object*)in_object_resource)//todo:fix me
		, m_resource_name(in_source_name)
		, m_uv_region(ForceInit)
		, m_draw_as(in_draw_type)
		, m_tiling(in_tiling)
		, m_mirroring(ESlateBrushMirrorType::NoMirror)
		, m_image_type(in_image_type)
	{

	}

	FSlateBrush::FSlateBrush(ESlateBrushDrawType::Type in_draw_type, const DoDoUtf8String in_resource_name,
		const FMargin& in_margin, ESlateBrushTileType::Type in_tiling, ESlateBrushImageType::Type in_image_type,
		const glm::vec2& in_image_size, const FSlateColor& in_tint, void* in_object_resource,
		bool b_in_dynamically_loaded)
		: m_image_size(in_image_size)
		, m_margin(in_margin)
		, m_tint_color(in_tint)
		, m_resource_object((Object*)in_object_resource)//todo:fix me
		, m_resource_name(in_resource_name)
		, m_uv_region(ForceInit)
		, m_draw_as(in_draw_type)
		, m_tiling(in_tiling)
		, m_mirroring(ESlateBrushMirrorType::NoMirror)
		, m_image_type(in_image_type)
	{
	}
}
