#pragma once

#include "CompositeFont.h"//FFontFaceDataConstPtr
#include "Core/String/DoDoString.h"//DoDoUtf8String
#include <set>
#include "freetype/freetype.h"

#include <fstream>//todo:may use other io in different platform

#include <type_traits>

#include <unordered_map>

#include <functional>

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
	* provides low-level advance caching to avoid repeated calls to FT_Get_Advance
	*/
	class FFreeTypeAdvanceCache
	{
	public:
		FFreeTypeAdvanceCache(FT_Face in_face, const int32_t in_load_flags, const int32_t in_font_size, const float in_font_scale);

		bool find_or_cache(const uint32_t in_glyph_index, FT_Fixed& out_cached_advance);
	private:
		FT_Face m_face;
		const int32_t m_load_flags;
		const int32_t m_font_size;
		const float m_font_scale;
		std::map<uint32_t, FT_Fixed> m_advance_map;
	};		

	//class FFreeTypeCacheDirectory::FFontKey;
	class FFontKey
	{
	public:
		FFontKey(FT_Face in_face, const int32_t in_flags, const int32_t in_font_size, const float in_font_scale)
			: m_face(in_face)
			, m_flags(in_flags)
			, m_font_size(in_font_size)
			, m_font_scale(in_font_scale)
			, m_key_hash(0)
		{
			m_key_hash = Get_Type_Hash((uint64_t)(void*)m_face);//todo:fix me
			m_key_hash = Hash_Combine(m_key_hash, Get_Type_Hash(m_flags));
			m_key_hash = Hash_Combine(m_key_hash, Get_Type_Hash(m_font_size));
			m_key_hash = Hash_Combine(m_key_hash, Get_Type_Hash(m_font_scale));
		}

		bool operator==(const FFontKey& other) const
		{
			return m_face == other.m_face &&
				m_flags == other.m_flags &&
				m_font_size == other.m_font_size &&
				m_font_scale == other.m_font_scale;
		}

		friend inline int32_t Get_Type_Hash(const FFontKey& key)
		{
			return key.m_key_hash;
		}
	private:
		FT_Face m_face;
		int32_t m_flags;
		int32_t m_font_size;
		float m_font_scale;
		uint32_t m_key_hash;
	};
}

class DoDo::FFontKey;

template<>
struct std::hash<DoDo::FFontKey>
{
	std::size_t operator()(const DoDo::FFontKey& key) const
	{
		return Get_Type_Hash(key);
	}
};

namespace DoDo {
	
	/*
	* class that manages directory of caches for advances, kerning, and other parameters for fonts
	*/
	class FFreeTypeCacheDirectory
	{
	public:
		/*
		* retrieve the advance cache for a given set of font parameters
		* @return a reference to the font advance cache
		*/
		std::shared_ptr<FFreeTypeAdvanceCache> get_advance_cache(FT_Face in_face, const int32_t in_load_flags, const int32_t in_font_size, const float in_font_scale);
		
	private:

		std::unordered_map<FFontKey, std::shared_ptr<FFreeTypeAdvanceCache>> m_advance_cache_map;
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


