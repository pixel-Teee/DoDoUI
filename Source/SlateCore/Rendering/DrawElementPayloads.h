#pragma once

//#include "DrawElements.h"//FSlateBoxPayload depends on it
#include "Core/Math/Box2D.h"//FSlateBoxPayload depends on it

#include "glm/vec4.hpp"
#include "SlateCore/Fonts/SlateFontInfo.h"//FSlateFontInfo depends on it
#include "SlateCore/Layout/Margin.h"//FSlatBoxPayload depends on it
#include "SlateCore/Styling/SlateBrush.h"//FSlateBoxPayload depends on it

#include "RenderingCommon.h"//FSlateViewportPayload depends on ISlateViewport


#include "SlateCore/Fonts/ShapedTextFwd.h"//FShapedGlyphSequencePtr depends on it

namespace DoDo
{
	enum class ETextOverflowDirection : uint8_t
	{
		//no overflow
		NoOverflow,
		//left justification overflow
		LeftToRight,
		//right justification overflow
		RightToLeft
	};

	struct FSlateGradientStop
	{
		glm::vec2 m_position;
		FLinearColor m_color;

		/*
		 * construct a gradient stop from a position and a color
		 *
		 * @param InPosition - the position in widget space for this stop, both x and y are used for a single-axis gradient
		 *						a two stop gradient should go from (0, 0) to (width, height)
		 *
		 * @param InColor	 - the color to lerp towards at this stop
		 */
		FSlateGradientStop(const glm::vec2& in_position, const FLinearColor& in_color)
			: m_position(in_position)
			, m_color(in_color)
		{}
	};

	class FSlateShaderResourceProxy;
	//class FSlateBrush;

	struct FSlateDataPayload
	{
		virtual ~FSlateDataPayload() {}

		//todo:implement add referenced objects
	};

	struct FSlateTintableElement
	{
		FLinearColor tint;//tint

		void set_tint(const FLinearColor& in_tint) { tint = in_tint; }

		FLinearColor get_tint() const { return tint; }
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

	struct FSlateRoundedBoxPayload : public FSlateBoxPayload
	{
		FLinearColor m_outline_color;
		glm::vec4 m_radius;
		float m_outline_weight;

		void set_radius(glm::vec4 in_radius) { m_radius = in_radius; }
		glm::vec4 get_radius() const { return m_radius; }

		void set_outline(const FLinearColor& in_outline_color, float in_outline_weight) { m_outline_color = in_outline_color; m_outline_weight = in_outline_weight; }
		FLinearColor get_outline_color() const { return m_outline_color; }
		float get_outline_weight() const { return m_outline_weight; }
	};

	struct FSlateTextPayload : public FSlateDataPayload, public FSlateTintableElement
	{
		//the font to use when rendering
		FSlateFontInfo m_font_info;

		//basic text data
		DoDoUtf8String m_immutable_text;

		int32_t get_text_length() const { return m_immutable_text.get_length(); }

		const FSlateFontInfo& get_font_info() const { return m_font_info; }

		const DoDoUtf8String& get_text() const { return m_immutable_text; }

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

	struct FTextOverflowArgs
	{
		FTextOverflowArgs(FShapedGlyphSequencePtr& in_oveflow_text, ETextOverflowDirection in_overflow_direction)
			: m_overflow_text_ptr(in_oveflow_text)
			, m_overflow_direction(in_overflow_direction)
			, m_b_force_ellipsis_due_to_clipped_line(false)
		{}

		FTextOverflowArgs()
			: m_overflow_direction(ETextOverflowDirection::NoOverflow)
			, m_b_force_ellipsis_due_to_clipped_line(false)
		{}

		/*sequence that represents the ellipsis glyph*/
		FShapedGlyphSequencePtr m_overflow_text_ptr;
		ETextOverflowDirection m_overflow_direction;
		bool m_b_force_ellipsis_due_to_clipped_line;
	};

	struct FSlateShapedTextPayload : public FSlateDataPayload, public FSlateTintableElement
	{
		//shaped text data
		FShapedGlyphSequencePtr m_shaped_glyph_sequence;

		FLinearColor m_outline_tint;

		FTextOverflowArgs m_overflow_args;

		const FShapedGlyphSequencePtr& get_shaped_glyph_sequence() const { return m_shaped_glyph_sequence; }

		FLinearColor get_outline_tint() const { return m_outline_tint; }

		void set_shaped_text(const FShapedGlyphSequencePtr& in_shaped_glyph_sequence, const FLinearColor& in_outline_tint)
		{
			m_shaped_glyph_sequence = in_shaped_glyph_sequence;
			m_outline_tint = in_outline_tint;
		}

		void set_overflow_args(const FTextOverflowArgs& in_args)
		{
			m_overflow_args = in_args;
		}
	};

	struct FSlateGradientPayload : public FSlateDataPayload
	{
		std::vector<FSlateGradientStop> m_gradient_stops;
		EOrientation m_gradient_type;
		glm::vec4 m_corner_radius;

		void set_gradient(const std::vector<FSlateGradientStop>& in_gradient_stops, EOrientation in_gradient_type, glm::vec4 in_corner_radius)
		{
			m_gradient_stops = in_gradient_stops;
			m_gradient_type = in_gradient_type;
			m_corner_radius = in_corner_radius;
		}
	};

	struct FSlateViewportPayload : public FSlateDataPayload, public FSlateTintableElement
	{
		FSlateShaderResource* m_render_target_resource;
		uint8_t m_b_allow_viewport_scaling : 1;
		uint8_t m_b_viewport_texture_alphy_only : 1;
		uint8_t m_b_requires_vsync : 1;

		void set_viewport(const std::shared_ptr<const ISlateViewport>& in_viewport, const FLinearColor& in_tint)
		{
			tint = in_tint;
			m_render_target_resource = in_viewport->get_viewport_render_target_texture();
			m_b_allow_viewport_scaling = in_viewport->allow_scaling();
			m_b_viewport_texture_alphy_only = in_viewport->is_viewport_texture_alpha_only();
			m_b_requires_vsync = in_viewport->requires_vsync();
		}
	};
}
