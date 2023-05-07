#pragma once

#include "SlateCore/Fonts/ShapedTextFwd.h"//FShapedGlyphSequencePtr depends on it

#include "Core/Containers/UnrealString.h"//FTextRange depends on it

#include "SlateCore/Fonts/SlateFontInfo.h"//FSlateFontInfo depends on it

#include "Slate/Framework/Text/IRun.h"//FShapedTextContext depends on it

#include "ShapedTextCacheFwd.h"

#include <unordered_map>

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

}

template<>
struct std::hash<DoDo::FCachedShapedTextKey>
{
	std::size_t operator()(const DoDo::FCachedShapedTextKey& key) const
	{
		return Get_Type_Hash(key);
	}
};

namespace DoDo{
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

		/*
		* try and find an existing shaped text instance
		* 
		* @param InKey the key identifying the shaped text instance to find
		* 
		* @return the shaped text instance, or null if it wasn't found or was stale
		*/
		FShapedGlyphSequencePtr find_shaped_text(const FCachedShapedTextKey& in_key) const;

		/*
		* try and find an existing shaped text instance, or add a new entry to the cache if one cannot be found
		* 
		* @param InKey the key identifying the shaped text instance to find or add
		* @param InText the text to shape if we can't find the shaped text in the cache, InKey may specify a sub-section of the entire text
		* @param InTextDirection the text direction of all of the text to be shaped, if present we do a unidirectional shape, otherwise we do a bidirectional shape
		* 
		* @return the shaped text instance
		*/
		FShapedGlyphSequencePtr find_or_add_shaped_text(const FCachedShapedTextKey& in_key, const DoDoUtf8String& in_text);

		/*
		* add the given shaped text instance to the cache, or generate a new instance and add that based on the parameters provided
		* 
		* @param InKey the key identifying the shaped text instance to add
		* @param InText the text to shape, InKey may specify a sub-section of the entire text
		* @param InTextDirection the text direction of all of the text to be shaped, if present we do a unidirectional shape, otherwise we do a bidirectional shape
		* @param InShapedText the shaped text instance to add
		* 
		* @return the shaped text instance
		*/
		FShapedGlyphSequencePtr add_shaped_text(const FCachedShapedTextKey& in_key, const DoDoUtf8String& in_text);

		FShapedGlyphSequencePtr add_shaped_text(const FCachedShapedTextKey& in_key, FShapedGlyphSequencePtr in_shaped_text);

	private:
		/*font cache to use when shaping text*/
		std::weak_ptr<FSlateFontCache> m_font_cache_ptr;

		/*mapping between a cache key and the corresponding shaped text*/
		std::unordered_map<FCachedShapedTextKey, FShapedGlyphSequencePtr> m_cached_shaped_text;
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
