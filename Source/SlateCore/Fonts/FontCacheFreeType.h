#pragma once

#include "CompositeFont.h"//FFontFaceDataConstPtr
#include "Core/String/DoDoString.h"//DoDoUtf8String
#include <set>
#include "freetype/freetype.h"

#include <fstream>//todo:may use other io in different platform

#include <type_traits>

namespace DoDo
{
	namespace FreeTypeUtils
	{
		template<typename TRetType, typename TParamType>
		typename std::enable_if<std::is_integral<TParamType>::value, TRetType>::type Convert26Dot6ToRoundedPixel(TParamType InValue)
		{
			return static_cast<TRetType>((InValue + (1 << 5)) >> 6);
		}

		/*covert the given value from 26.6 space into rounded pixel space*/
		template<typename TRetType, typename TParamType>
		typename std::enable_if<std::is_floating_point<TParamType>::value, TRetType>::type Convert26Dot6ToRoundedPixel(TParamType InValue)//check floating
		{
			return static_cast<TRetType>(InValue / 64.0f);
		}

		template<typename TRetType, typename TParamType>
		typename std::enable_if<std::is_integral<TParamType>::value, TRetType>::type ConvertPixelTo26Dot26(TParamType InValue)
		{
			return static_cast<TRetType>(InValue << 6);//* 64
		}

		template<typename TRetType, typename TParamType>
		typename std::enable_if<std::is_floating_point<TParamType>::value, TRetType>::type ConvertPixelTo16Dot16(TParamType InValue)
		{
			return static_cast<TRetType>(InValue * 65536);
		}

		void apply_size_and_scale(FT_Face in_face, const int32_t in_font_size, const float in_font_scale);
		/*
		* load the given glyph into the active slot of the given face
		*/
		FT_Error load_glyph(FT_Face in_face, const uint32_t in_glyph_index, const int32_t in_load_flags, const int32_t in_font_size, const float in_font_scale);
	}
	/*
	 * wrapped around a free type library instance
	 * this instance will be create using our memory allocator
	 */
	class FFreeTypeLibrary
	{
	public:
		FFreeTypeLibrary();
		~FFreeTypeLibrary();

		FT_Library get_library() const
		{
			return m_ft_library;
		}

	private:
		//non-copyable
		FFreeTypeLibrary(const FFreeTypeLibrary&);
		FFreeTypeLibrary& operator=(const FFreeTypeLibrary&);

		FT_Library m_ft_library;
		//FT_Memory m_custom_memory;
	};
	/*
	 * wrapped around a free type face instance
	 * it will either steal the given buffer, or stream the given file from disk
	 */
	class FFreeTypeFace
	{
	public:
		FFreeTypeFace(const FFreeTypeLibrary* in_ft_library, const DoDoUtf8String& in_file_name, const int32_t in_face_index);

		~FFreeTypeFace();

		float get_bit_map_render_scale() const
		{
			//todo:implement free type utils
			//return FreeTypeUtils::get_bit_map_render_scale(m_ft_face);

			return 1.0f;
		}

		bool is_face_valid()
		{
			return m_ft_face != nullptr;
		}

		FT_Face get_face() const
		{
			return m_ft_face;
		}
	private:

		//non-copyable
		FFreeTypeFace(const FFreeTypeFace&);
		FFreeTypeFace& operator=(const FFreeTypeFace&);

		FT_Face m_ft_face;//free type face

		FFontFaceDataConstPtr m_memory;//just a uint8_t memory block

		/*custom FreeType stream handler for reading font data via the unreal file system*/
		struct FFTStreamHandler
		{
			FFTStreamHandler();

			FFTStreamHandler(const DoDoUtf8String& in_file_name);

			static void close_file(FT_Stream in_stream);

			static unsigned long read_data(FT_Stream in_stream, unsigned long in_offset, unsigned char* in_buffer, unsigned long in_count);

			std::ifstream m_file_handle;
			int64_t m_font_size_bytes;
		};

		FFTStreamHandler m_ft_stream_handler;

		FT_StreamRec m_ft_stream;

		FT_Open_Args m_ft_face_open_args;

		std::set<DoDoUtf8String>  m_attributes;

		EFontLayoutMethod m_layout_method;
	};
}