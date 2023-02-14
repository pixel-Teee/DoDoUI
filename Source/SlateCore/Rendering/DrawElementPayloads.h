#pragma once

//#include "DrawElements.h"//FSlateBoxPayload depends on it
#include "Core/Math/Box2D.h"//FSlateBoxPayload depends on it

#include "glm/vec4.hpp"
#include "SlateCore/Fonts/SlateFontInfo.h"//FSlateFontInfo depends on it
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
			FSlateResourceHandle handle = in_brush->get_rendering_resource(local_size, draw_scale);//todo:find rendering resource from texture map

			if (handle.is_valid())
			{
				m_resource_proxy = handle.get_resource_proxy();
			}
			else
			{
				m_resource_proxy = nullptr;
			}
		}

		virtual ~FSlateBoxPayload()
		{
			
		}
	};

	struct FSlateTextPayload : public FSlateDataPayload, public FSlateTintableElement
	{
		//the font to use when rendering
		FSlateFontInfo m_font_info;

		//basic text data
		DoDoUtf8String m_immutable_text;

		int32_t get_text_length() const { return m_immutable_text.get_length(); }

		void set_text(const DoDoUtf8String& in_text, const FSlateFontInfo& in_font_info, int32_t in_start_index, int32_t in_end_index)
		{
			//todo:implement this function
		}

		void set_text(const DoDoUtf8String& in_text, const FSlateFontInfo& in_font_info)
		{
			m_font_info = in_font_info;
			m_immutable_text = in_text;
		}
	};
}
