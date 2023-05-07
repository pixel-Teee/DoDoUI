#pragma once

#include "SlateCore/Fonts/ShapedTextFwd.h"//FShapedGlyphSequencePtr depends on it

#include "Core/Containers/UnrealString.h"//FTextRange depends on it

#include "SlateCore/Fonts/SlateFontInfo.h"//FSlateFontInfo depends on it

#include "Slate/Framework/Text/IRun.h"//FShapedTextContext depends on it

#include "ShapedTextCacheFwd.h"

namespace DoDo {
	/*information representing a piece of shaped text*/
	struct FCachedShapedTextKey
	{
	public:
		FCachedShapedTextKey(const FTextRange& in_text_range, const float in_scale, const FShapedTextContext& in_text_context, const FSlateFontInfo& in_font_info)
			: m_text_range(in_text_range)
			, m_scale(in_scale)
			, m_text_content(in_text_context)
			, m_font_info(in_font_info)
		{}

		bool operator==(const FCachedShapedTextKey& other) const
		{
			return m_text_range == other.m_text_range
				&& m_scale == other.m_scale
				&& m_text_content == other.m_text_content
				&& m_font_info.is_identical_to_for_caching(other.m_font_info);
		}

		bool operator!=(const FCachedShapedTextKey& other) const
		{
			return !(*this == other);
		}

		friend inline uint32_t Get_Type_Hash(const FCachedShapedTextKey& key)
		{
			uint32_t key_hash = 0;
			key_hash = Hash_Combine(key_hash, Get_Type_Hash(key.m_text_range));
			key_hash = Hash_Combine(key_hash, Get_Type_Hash(key.m_scale));
			key_hash = Hash_Combine(key_hash, Get_Type_Hash(key.m_text_content));
			key_hash = Hash_Combine(key_hash, Get_Type_Hash(key.m_font_info));

			return key_hash;
		}
		FTextRange m_text_range;
		float m_scale;
		FShapedTextContext m_text_content;//note:just two enumerates
		FSlateFontInfo m_font_info;
	};

	class FSlateFontCache;
	/*a map of FShapedGlyphSequence*/
	/*cache of shaped text*/
	class FShapedTextCache
	{
	public:
		/*create a new shaped text cache*/
		static FShapedTextCachePtr Create(const std::shared_ptr<FSlateFontCache>& in_font_cache)
		{
			return std::make_shared<FShapedTextCache>(in_font_cache);
		}

		FShapedTextCache(const std::shared_ptr<FSlateFontCache>& in_font_cache)
			: m_font_cache_ptr(in_font_cache)
		{}

	private:
		/*font cache to use when shaping text*/
		std::weak_ptr<FSlateFontCache> m_font_cache_ptr;

		/*mapping between a cache key and the corresponding shaped text*/
		std::map<FCachedShapedTextKey, FShapedGlyphSequencePtr> m_cached_shaped_text;
	};

	/*utility functions that can provide efficient caching of common operations*/
	namespace ShapedTextCacheUtil
	{
		/*
		* extract a sub-section of a run of text into its own shaped glyph sequence
		* 
		* 
		*/
		FShapedGlyphSequencePtr get_shaped_text_sub_sequence(const FShapedTextCachePtr& in_shaped_text_cache, const FCachedShapedTextKey& in_run_key,
			const FTextRange& in_text_range, const DoDoUtf8String& in_text, const TextBiDi::ETextDirection in_text_direction);
	}
}