#pragma once

//#include "DrawElements.h"//FSlateBoxPayload depends on it
#include "Core/Math/Box2D.h"//FSlateBoxPayload depends on it

#include "glm/vec4.hpp"
#include "SlateCore/Layout/Margin.h"//FSlatBoxPayload depends on it
#include "SlateCore/Styling/SlateBrush.h"//FSlateBoxPayload depends on it

namespace DoDo
{
	class FSlateShaderResourceProxy;
	//class FSlateBrush;

	struct FSlateDataPayload
	{
		virtual ~FSlateDataPayload() {}

		//todo:implement add referenced objects
	};

	struct FSlateTintableElement
	{
		glm::vec4 tint;//tint

		void set_tint(const glm::vec4& in_tint) { tint = in_tint; }

		glm::vec4 get_tint() const { return tint; }
	};

	struct FSlateBoxPayload : public FSlateDataPayload, public FSlateTintableElement
	{
		FMargin m_margin;

		FBox2D m_uv_region;

		const FSlateShaderResourceProxy* m_resource_proxy;

		//todo:implement FSlateShaderResourceProxy
		ESlateBrushTileType::Type m_tiling;

		ESlateBrushMirrorType::Type m_mirroring;

		ESlateBrushDrawType::Type m_draw_type;

		const FMargin& get_brush_margin() const { return m_margin; }

		const FBox2D& get_brush_uv_region() const { return m_uv_region; }

		ESlateBrushTileType::Type get_brush_tiling() const { return m_tiling; }

		ESlateBrushMirrorType::Type get_brush_mirroring() const { return m_mirroring; }

		ESlateBrushDrawType::Type get_brush_draw_type() const { return m_draw_type; }

		const FSlateShaderResourceProxy* get_resource_proxy() const { return  m_resource_proxy; }

		//todo:implement get resource proxy

		void set_brush(const FSlateBrush* in_brush, glm::vec2 local_size, float draw_scale)
		{
			m_margin = in_brush->get_margin();

			m_tiling = in_brush->get_tiling();

			m_mirroring = in_brush->get_mirroring();

			m_draw_type = in_brush->get_draw_type();

			//todo:from in_brush to get the shader resource proxy
		}

		virtual ~FSlateBoxPayload()
		{
			
		}
	};
}
