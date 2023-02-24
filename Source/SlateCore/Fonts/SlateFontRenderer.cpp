#include <PreCompileHeader.h>

#include "SlateFontRenderer.h"

#include "FontCache.h"

#include "freetype/ftbitmap.h"

namespace DoDo
{
	void SlateFontRendererUtils::append_glyph_flags(const FFreeTypeFace& in_face, const FFontData& in_font_data,
		uint32_t& in_out_glyph_flags)
	{
		//note:setup additional glyph flags
		in_out_glyph_flags |= FT_LOAD_TARGET_MONO | FT_LOAD_FORCE_AUTOHINT;
	}

	FSlateFontRenderer::FSlateFontRenderer(const FFreeTypeLibrary* in_ft_library,
		FCompositeFontCache* in_composite_font_cache)
			: m_ft_library(in_ft_library)
			, m_composite_font_cache(in_composite_font_cache)
	{
	}

	uint16_t FSlateFontRenderer::get_max_height(const FSlateFontInfo& in_font_info, const float in_scale) const
	{
		return 0;//todo:implement this function
	}
	bool FSlateFontRenderer::get_render_data(const FShapedGlyphEntry& in_shaped_glyph, const FFontOutlineSettings& in_outline_settings, FCharacterRenderData& out_render_data) const
	{
		//todo:implement this function

		FFreeTypeFaceGlyphData face_glyph_data;
		face_glyph_data.m_face_and_memory = in_shaped_glyph.m_font_face_data->m_font_face.lock();
		face_glyph_data.m_glyph_index = in_shaped_glyph.m_graph_index;
		face_glyph_data.m_glyph_flags = in_shaped_glyph.m_font_face_data->m_glyph_flags;

		if (face_glyph_data.m_face_and_memory)
		{
			if (face_glyph_data.m_face_and_memory->is_face_valid())
			{
				FT_Error error = FreeTypeUtils::load_glyph(face_glyph_data.m_face_and_memory->get_face(),//face is across a font file to load
					face_glyph_data.m_glyph_index,//glyph index, across glyph index to access the bitmap
					face_glyph_data.m_glyph_flags,
					in_shaped_glyph.m_font_face_data->m_font_size,
					in_shaped_glyph.m_font_face_data->m_font_scale);

				if (error == 0)
				{
					return get_render_data_internal(face_glyph_data, in_shaped_glyph.m_font_face_data->m_font_scale, in_outline_settings, out_render_data);
				}
			}
		}

		return false;
	}
	bool FSlateFontRenderer::get_render_data_internal(const FFreeTypeFaceGlyphData& in_face_glyph_data, const float in_scale, const FFontOutlineSettings& in_outline_settings, FCharacterRenderData& out_render_data) const
	{
		FT_Face face = in_face_glyph_data.m_face_and_memory->get_face();//get the free type face

		//get the lot for the glyph, this contains measurement info
		FT_GlyphSlot slot = face->glyph;

		const float bit_map_atlas_scale = 1.0f;//todo:implement FreeTypeUtils::get_bitmap_atlas_scale

		float scaled_outline_size = in_outline_settings.m_outline_size * in_scale;

		out_render_data.m_b_is_gray_scale = true;
		out_render_data.m_b_supports_outline = FT_IS_SCALABLE(face);

		//todo:implement render outline

		//this path renders a standard font with no outline, this may occur if the outline failed to generate
		FT_Render_Glyph(slot, FT_RENDER_MODE_NORMAL);

		FT_Bitmap* bitmap = nullptr;
		FT_Bitmap tmp_bitmap;

		if (slot->bitmap.pixel_mode == FT_PIXEL_MODE_BGRA)
		{
			out_render_data.m_b_is_gray_scale = false;
			bitmap = &slot->bitmap;
		}

		const uint32_t bytes_per_pixel = out_render_data.m_b_is_gray_scale ? 1 : 4;
		out_render_data.m_raw_pixels.clear();
		out_render_data.m_raw_pixels.resize(bitmap->rows * bitmap->width * bytes_per_pixel, 0);

		//nothing to do for zero width or height glyphs
		if (out_render_data.m_raw_pixels.size())
		{
			//copy the rendered bitmap to our raw pixels array
			for (uint32_t row = 0; row < bitmap->rows; ++row)
			{
				//copy a single row, not bitmap.pitch contains the offset (in bytes) between rows, not always equal to bitmap.width!
				std::memcpy(&out_render_data.m_raw_pixels[row * bitmap->width * bytes_per_pixel], &bitmap->buffer[row * bitmap->pitch], bitmap->width * bytes_per_pixel);
			}

			//grayscale images not using 256 colors need to covert their gray range to a 0-255 range
			if (out_render_data.m_b_is_gray_scale && bitmap->num_grays != 256)
			{
				const int32_t gray_boost = 255 / (bitmap->num_grays - 1);
				for (uint8_t& raw_pixel : out_render_data.m_raw_pixels)
				{
					raw_pixel *= gray_boost;
				}
			}
		}

		out_render_data.m_size_x = (int16_t)bitmap->width;
		out_render_data.m_size_y = (int16_t)bitmap->rows;

		if (bitmap == &tmp_bitmap)
		{
			FT_Bitmap_Done(m_ft_library->get_library(), bitmap);//destroy bitmap
			bitmap = nullptr;
		}

		//resize the generated bitmap if required
		
		const int32_t horizontal_offset = slot->bitmap_left * bit_map_atlas_scale;
		const int32_t vertical_offset = (slot->bitmap_top + scaled_outline_size) * bit_map_atlas_scale;
		
		out_render_data.m_horizontal_offset = (int16_t)horizontal_offset;
		out_render_data.m_vertical_offset = (int16_t)vertical_offset;

		return true;
	}

	FFreeTypeFaceGlyphData FSlateFontRenderer::get_font_face_for_code_point(const FFontData& in_font_data,
		const uint32_t in_code_point, EFontFallback max_fall_back_level) const
	{
		FFreeTypeFaceGlyphData return_val;

		//try the requested font first
		{
			return_val.m_face_and_memory = m_composite_font_cache->get_font_face(in_font_data);

			if(return_val.m_face_and_memory)
			{
				return_val.m_glyph_index = FT_Get_Char_Index(return_val.m_face_and_memory->get_face(), in_code_point);//to get glyph index and switch face state to code point
				return_val.m_char_fall_back_level = EFontFallback::FF_NoFallback;
			}
		}

		//if we have valid face and memory but it just hasn't finished loading
		//return like we found it, so that we don't immediately trigger falling back to yet another font
		//when it may have the glyph once it's actually done loading
		//if(return_val.m_face_and_memory && return_val.m_face_and_memory->is_face_loading())
		//{
		//	
		//}

		return return_val;
	}
}
