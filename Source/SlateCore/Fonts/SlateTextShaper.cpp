#include "PreCompileHeader.h"

#include "SlateTextShaper.h"

#include "FontCache.h"//FShapedGlyphEntry depends on it

namespace DoDo {
	namespace {
		bool render_code_point_as_white_space(const DoDoUtf8String& in_code_point)
		{ 
			return  DoDoUtf8String::is_white_space(in_code_point) ||
				in_code_point == DoDoUtf8String(u8"\u200B") || //zero width space
				in_code_point == DoDoUtf8String(u8"\u2009") || //thin space
				in_code_point == DoDoUtf8String(u8"\u202F"); //narrow no-break space
		}
	}
	FSlateTextShaper::FSlateTextShaper(FFreeTypeCacheDirectory* in_ft_cache_directory, FCompositeFontCache* in_composite_font_cache, FSlateFontRenderer* in_font_renderer, FSlateFontCache* in_font_cache)
		: m_ft_cache_directory(in_ft_cache_directory)
		, m_composite_font_cache(in_composite_font_cache)
		, m_font_renderer(in_font_renderer)
		, m_font_cache(in_font_cache)
		, m_text_bidi_detection(TextBiDi::create_text_bidi())//icu
	{
	}
	FShapedGlyphSequencePtr FSlateTextShaper::shape_bidirectional_text(const DoDoUtf8String& in_text, const int32_t in_text_start, const int32_t in_text_len, const FSlateFontInfo& in_font_info, const float in_font_scale, const TextBiDi::ETextDirection in_base_direction, const ETextShapingMethod text_shaping_method) const
	{
		std::vector<TextBiDi::FTextDirectionInfo> text_direction_infos;

		//note:output the FTextDirectionInfo array
		m_text_bidi_detection->compute_text_direction(in_text, in_text_start, in_text_len, in_base_direction, text_direction_infos);

		std::vector<FShapedGlyphEntry> glyphs_to_render;
		for (const TextBiDi::FTextDirectionInfo& text_direction_info : text_direction_infos)
		{
			perform_text_shaping(in_text, text_direction_info.m_start_index, text_direction_info.m_length, in_font_info, in_font_scale, text_direction_info.m_text_direction,
				text_shaping_method, glyphs_to_render);
		}

		return finalize_text_shaping(std::move(glyphs_to_render), in_font_info, in_font_scale, FSourceTextRange(in_text_start, in_text_len));
	}
	void FSlateTextShaper::perform_text_shaping(const DoDoUtf8String& in_text, const int32_t in_text_start, const int32_t in_text_len, const FSlateFontInfo& in_font_info, const float in_font_scale, const TextBiDi::ETextDirection in_text_direction, const ETextShapingMethod text_shaping_method, std::vector<FShapedGlyphEntry>& out_glyphs_to_render) const
	{
		//todo:check text direction is not mixed

		if (in_text_len > 0)
		{
			auto text_requires_full_shaping = [&]()->bool
			{
				//rtl text always requires full shaping
				if (in_text_direction == TextBiDi::ETextDirection::RightToLeft)
				{
					return true;
				}

				//ltr text containing certain scripts or surrogate pairs requirs full shaping
				{
					//note:we deliberately avoid using harfbuzz/icu here as we don't care about the script itself
					//only that the character is within a shaped script range (and testing that is much faster!)
					auto char_requires_full_shaping = [](const DoDoUtf8String& in_char)->bool
					{
						//note:this isn't an exhaustive list, as it omits some "dead" or uncommon languages, and ranges
						//outside the bmp
						#define RETURN_TRUE_IF_CHAR_WITHIN_RANGE(LOWER, UPPER) if(const_cast<DoDoUtf8String&>(in_char).to_utf16_code_point(0) >= (LOWER) && const_cast<DoDoUtf8String&>(in_char).to_utf16_code_point(0) <= (UPPER)) return true

						//zero-width joiner
						if (const_cast<DoDoUtf8String&>(in_char).to_utf16_code_point(0) == u'\u200D')
						{
							return true;
						}

						//combining characters
						RETURN_TRUE_IF_CHAR_WITHIN_RANGE(u'\u0300', u'\u036f');
						RETURN_TRUE_IF_CHAR_WITHIN_RANGE(u'\u1AB0', u'\u1AFF');
						RETURN_TRUE_IF_CHAR_WITHIN_RANGE(u'\u1DC0', u'\u1DFF');
						RETURN_TRUE_IF_CHAR_WITHIN_RANGE(u'\u20D0', u'\u20FF');
						RETURN_TRUE_IF_CHAR_WITHIN_RANGE(u'\u3099', u'\u309A');
						RETURN_TRUE_IF_CHAR_WITHIN_RANGE(u'\u31C0', u'\u31EF');
						RETURN_TRUE_IF_CHAR_WITHIN_RANGE(u'\uFE20', u'\uFE2F');

						//devanagari
						RETURN_TRUE_IF_CHAR_WITHIN_RANGE(u'\u0900', u'\u097F');
						RETURN_TRUE_IF_CHAR_WITHIN_RANGE(u'\uA8E0', u'\uA8FF');
						RETURN_TRUE_IF_CHAR_WITHIN_RANGE(u'\u1CD0', u'\u1CFF');

						//telugu
						RETURN_TRUE_IF_CHAR_WITHIN_RANGE(u'\u0C00', u'\u0C7F');

						//thai
						RETURN_TRUE_IF_CHAR_WITHIN_RANGE(u'\u0E00', u'\u0E7F');

						//tibetan
						RETURN_TRUE_IF_CHAR_WITHIN_RANGE(u'\u0F00', u'\u0FFF');

						//khmer
						RETURN_TRUE_IF_CHAR_WITHIN_RANGE(u'\u1780', u'\u17FF');
						RETURN_TRUE_IF_CHAR_WITHIN_RANGE(u'\u19E0', u'\u19FF');

						//sinhala
						RETURN_TRUE_IF_CHAR_WITHIN_RANGE(u'\u0D80', u'\u0DFF');

						//limbu
						RETURN_TRUE_IF_CHAR_WITHIN_RANGE(u'\u1900', u'\u194F');

						//tai tham
						RETURN_TRUE_IF_CHAR_WITHIN_RANGE(u'\u1A20', u'\u1AAF');

						//tai viet
						RETURN_TRUE_IF_CHAR_WITHIN_RANGE(u'\uAA80', u'\uAADF');

						//batak
						RETURN_TRUE_IF_CHAR_WITHIN_RANGE(u'\u1BC0', u'\u1BFF');
						#undef RETURN_TRUE_IF_CHAR_WITHIN_RANGE

						return false;
					};

					const int32_t text_end_index = in_text_start + in_text_len;
					for (int32_t running_text_index = in_text_start; running_text_index < text_end_index; ++running_text_index)
					{
						DoDoUtf8String Char = in_text.utf8_at(running_text_index);

						//char combine to 
						if (Char.utf8_code_at(0) <= u8'\u007F')
						{
							continue;
						}

						if (char_requires_full_shaping(Char))
						{
							return true;
						}

						//todo:check is 4 bytes
					}
				}

				return false;
			};

			if (text_shaping_method == ETextShapingMethod::FullShaping || (text_shaping_method == ETextShapingMethod::Auto && text_requires_full_shaping()))
			{
				//perform_harf_buzz_text_shaping(in_text, in_text_start, in_text_len, in_font_info, in_font_scale, in_text_direction, out_glyphs_to_render);
			}
			else
			{
				perform_kerning_only_text_shaping(in_text, in_text_start, in_text_len, in_font_info, in_font_scale, out_glyphs_to_render);
			}
		}
	}
	FShapedGlyphSequencePtr FSlateTextShaper::finalize_text_shaping(std::vector<FShapedGlyphEntry> in_glyphs_to_render, const FSlateFontInfo& in_font_info, const float in_font_scale, const FSourceTextRange& in_source_text_range) const
	{
		int16_t text_base_line = 0;
		uint16_t max_height = 0;

		{
			//just get info for the null character
			DoDoUtf8String Char(" ");
			const FFontData& font_data = m_composite_font_cache->get_default_font_data(in_font_info);
			const FFreeTypeFaceGlyphData face_glyph_data = m_font_renderer->get_font_face_for_code_point(font_data, Char.to_utf16_code_point(0), in_font_info.m_font_fallback);

			if (face_glyph_data.m_face_and_memory && face_glyph_data.m_face_and_memory->is_face_valid())
			{
				FreeTypeUtils::apply_size_and_scale(face_glyph_data.m_face_and_memory->get_face(), in_font_info.m_size, in_font_scale);

				text_base_line = FreeTypeUtils::Convert26Dot6ToRoundedPixel<int16_t>(face_glyph_data.m_face_and_memory->get_descender());
				max_height = FreeTypeUtils::Convert26Dot6ToRoundedPixel<int16_t>(face_glyph_data.m_face_and_memory->get_scaled_height());
			}
		}

		return std::make_shared<FShapedGlyphSequence>(std::move(in_glyphs_to_render), text_base_line, max_height, in_font_info.m_outline_settings, in_source_text_range);
	}
	void FSlateTextShaper::perform_kerning_only_text_shaping(const DoDoUtf8String& in_text, const int32_t in_text_start, const int32_t in_text_len, const FSlateFontInfo& in_font_info, const float in_font_scale, std::vector<FShapedGlyphEntry>& out_glyphs_to_render) const
	{
		//we need to work out the correct FFontData for everything so that we can build accurate FShapedGlyphFaceData for rendering later on
		std::vector<FKerningOnlyTextSequenceEntry> kerning_only_text_sequence;

		//step 1) split the text into sections that are using the same font face (composite fonts may contain different faces for different character ranges)
		{
			//data used while detecting font face boundaries
			int32_t split_start_index = in_text_start;
			int32_t running_text_index = in_text_start;
			const FFontData* running_font_data_ptr = nullptr;
			std::shared_ptr<FFreeTypeFace> running_face_and_memory;
			float running_sub_font_scaling_factor = 1.0f;

			auto append_pending_font_data_to_sequence = [&]()
			{
				if (running_font_data_ptr)
				{
					kerning_only_text_sequence.emplace_back(
						split_start_index, //in_text_start_index
						running_text_index - split_start_index, //in_text_length
						running_font_data_ptr, //in_font_data_ptr
						running_face_and_memory, //in_face_and_memory
						running_sub_font_scaling_factor //in_sub_font_scaling_factor
					);

					running_font_data_ptr = nullptr;
					running_face_and_memory.reset();
					running_sub_font_scaling_factor = 1.0f;
				}
			};

			const int32_t text_end_index = in_text_start + in_text_len;
			for (; running_text_index < text_end_index; ++running_text_index)
			{
				DoDoUtf8String current_char = in_text.utf8_at(running_text_index); //todo:fix me, use const

				const bool b_should_render_as_white_space = render_code_point_as_white_space(current_char);

				//first try with the actual character
				float sub_font_scaling_factor = 1.0f;
				const FFontData* font_data_ptr = &m_composite_font_cache->get_font_data_for_code_point(in_font_info, current_char.to_utf16_code_point(0), sub_font_scaling_factor);
				FFreeTypeFaceGlyphData face_glyph_data = m_font_renderer->get_font_face_for_code_point(*font_data_ptr, current_char.to_utf16_code_point(0), b_should_render_as_white_space ? EFontFallback::FF_NoFallback : in_font_info.m_font_fallback);

				//if none of our fonts can render that character (as the fallback font may be missing)
				//try again with the fallback character, or a normal space if this character was supposed to
				if (!face_glyph_data.m_face_and_memory)
				{
					DoDoUtf8String fallback_char = b_should_render_as_white_space ? DoDoUtf8String(" ") : DoDoUtf8String(u8"\uFFFD");
					font_data_ptr = &m_composite_font_cache->get_font_data_for_code_point(in_font_info, fallback_char.to_utf16_code_point(0), sub_font_scaling_factor);
					face_glyph_data = m_font_renderer->get_font_face_for_code_point(*font_data_ptr, fallback_char.to_utf16_code_point(0), in_font_info.m_font_fallback);
				}

				//only scalable font types can use sub-font scaling
				if (face_glyph_data.m_face_and_memory && !FT_IS_SCALABLE(face_glyph_data.m_face_and_memory->get_face()))
				{
					sub_font_scaling_factor = 1.0f;
				}

				if (!running_font_data_ptr || running_font_data_ptr != font_data_ptr || running_face_and_memory != face_glyph_data.m_face_and_memory || running_sub_font_scaling_factor != sub_font_scaling_factor)
				{
					append_pending_font_data_to_sequence();

					split_start_index = running_text_index;
					running_font_data_ptr = font_data_ptr;
					running_face_and_memory = face_glyph_data.m_face_and_memory;
					running_sub_font_scaling_factor = sub_font_scaling_factor;
				}
			}

			append_pending_font_data_to_sequence();
		}

		//step 2) now we use the font cache to get the size for each character, and kerning for each character pair
		{
			out_glyphs_to_render.resize(out_glyphs_to_render.size() + in_text_len);

			for (const FKerningOnlyTextSequenceEntry& kerning_only_text_sequence_entry : kerning_only_text_sequence)
			{
				if (!kerning_only_text_sequence_entry.m_face_and_memory)
				{
					continue;
				}

				const bool b_has_kerning = FT_HAS_KERNING(kerning_only_text_sequence_entry.m_face_and_memory->get_face()) != 0 || in_font_info.m_letter_spacing != 0;

				uint32_t glyph_flags = 0;
				SlateFontRendererUtils::append_glyph_flags(*kerning_only_text_sequence_entry.m_face_and_memory, *kerning_only_text_sequence_entry.m_font_data_ptr, glyph_flags);

				const float final_font_scale = in_font_scale * kerning_only_text_sequence_entry.m_sub_font_scaling_factor;

				//letter spacing should scale proportional to font size / 1000 (to roughly minic photoshp tracking)
				const float letter_spacing_scaled_as_float = in_font_info.m_letter_spacing != 0 ? in_font_info.m_letter_spacing * final_font_scale * in_font_info.m_size / 1000.0f : 0.0f;
				const int16_t letter_spacing_scaled = (int16_t)letter_spacing_scaled_as_float;

				FreeTypeUtils::apply_size_and_scale(kerning_only_text_sequence_entry.m_face_and_memory->get_face(), in_font_info.m_size, final_font_scale);
				std::shared_ptr<FShapedGlyphFaceData> shaped_glyph_face_data = std::make_shared<FShapedGlyphFaceData>(kerning_only_text_sequence_entry.m_face_and_memory, glyph_flags, in_font_info.m_size, final_font_scale);
				std::shared_ptr<FFreeTypeKerningCache> kerning_cache = m_ft_cache_directory->get_kerning_cache(kerning_only_text_sequence_entry.m_face_and_memory->get_face(), FT_KERNING_DEFAULT, in_font_info.m_size, final_font_scale);
				std::shared_ptr<FFreeTypeAdvanceCache> advance_cache = m_ft_cache_directory->get_advance_cache(kerning_only_text_sequence_entry.m_face_and_memory->get_face(), glyph_flags, in_font_info.m_size, final_font_scale);

				for (int32_t sequence_char_index = 0; sequence_char_index < kerning_only_text_sequence_entry.m_text_length; ++sequence_char_index)
				{
					const int32_t current_char_index = kerning_only_text_sequence_entry.m_text_start_index + sequence_char_index;
					DoDoUtf8String current_char = in_text.utf8_at(sequence_char_index);

					//todo:add insert substitute glyphs
					{
						uint32_t glyph_index = FT_Get_Char_Index(kerning_only_text_sequence_entry.m_face_and_memory->get_face(), current_char.to_utf16_code_point(0));

						//if the given font can't render that character (as the fallback font may be missing), try again with the fallback character
						if (current_char.to_utf16_code_point(0) != 0 && glyph_index == 0)
						{
							uint16_t code_point = u'\uFFFD';
							glyph_index = FT_Get_Char_Index(kerning_only_text_sequence_entry.m_face_and_memory->get_face(), code_point);
						}

						int16_t x_advance = 0;
						{
							FT_Fixed cached_advance_data = 0;
							if (advance_cache->find_or_cache(glyph_index, cached_advance_data))
							{
								x_advance = FreeTypeUtils::Convert26Dot6ToRoundedPixel<int16_t>((cached_advance_data + (1 << 9)) >> 10);
							}
						}

						out_glyphs_to_render.emplace_back();

						const int32_t current_glyph_entry_index = out_glyphs_to_render.size() - 1;

						FShapedGlyphEntry& shaped_glyph_entry = out_glyphs_to_render[current_glyph_entry_index];

						shaped_glyph_entry.m_font_face_data = shaped_glyph_face_data;
						shaped_glyph_entry.m_graph_index = glyph_index;//todo:fix this name
						shaped_glyph_entry.m_source_index = current_char_index;
						shaped_glyph_entry.m_x_advance = x_advance;
						shaped_glyph_entry.m_y_advance = 0;
						shaped_glyph_entry.m_x_offset = 0;
						shaped_glyph_entry.m_y_offset = 0;
						shaped_glyph_entry.m_num_characters_in_glyph = 1;
						shaped_glyph_entry.m_num_grapheme_clusters_in_glyph = 1;
						shaped_glyph_entry.m_text_direction = TextBiDi::ETextDirection::LeftToRight;
						shaped_glyph_entry.m_b_is_visible = !render_code_point_as_white_space(current_char);

						//apply the letter spacing and font kerning against the previous entry
						if (current_glyph_entry_index > 0 && b_has_kerning)
						{
							FShapedGlyphEntry& previous_shaped_glyph_entry = out_glyphs_to_render[current_glyph_entry_index - 1];

							if (letter_spacing_scaled != 0)
							{
								previous_shaped_glyph_entry.m_x_advance += letter_spacing_scaled;
							}

							if (shaped_glyph_entry.m_b_is_visible)
							{
								FT_Vector kerning_vector;
								if (kerning_cache && kerning_cache->find_or_cache(previous_shaped_glyph_entry.m_graph_index, shaped_glyph_entry.m_graph_index, kerning_vector))
								{
									const int8_t kerning = FreeTypeUtils::Convert26Dot6ToRoundedPixel<int8_t>(kerning_vector.x);
									previous_shaped_glyph_entry.m_x_advance += kerning;
									previous_shaped_glyph_entry.m_kerning = kerning;
								}
							}
						}
					}
				}
			}
		}
	}
}