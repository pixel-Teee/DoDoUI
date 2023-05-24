#include "PreCompileHeader.h"

#include "ShapedTextCache.h"

#include "SlateCore/Fonts/FontCache.h"//FShapedGlyphSequence depends on it

namespace DoDo {
	FShapedGlyphSequencePtr DoDo::ShapedTextCacheUtil::get_shaped_text_sub_sequence(const FShapedTextCachePtr& in_shaped_text_cache, const FCachedShapedTextKey& in_run_key, const FTextRange& in_text_range, const DoDoUtf8String& in_text, const TextBiDi::ETextDirection in_text_direction)
	{
		//get the shaped text for the entire run and try and make a sub-sequence from it - this can help minimize the amount of text shaping that needs to be done when drawing text
		FShapedGlyphSequencePtr shaped_text = in_shaped_text_cache->find_or_add_shaped_text(in_run_key, in_text);

		return shaped_text;
	}
	glm::vec2 ShapedTextCacheUtil::measure_shaped_text(const FShapedTextCachePtr& in_shaped_text_cache, const FCachedShapedTextKey& in_run_key, const FTextRange& in_measure_range, const DoDoUtf8String& in_text)
	{
		//get the shaped text for the entire run and try and take a sub-measurement from it - this can help minimize the amout of text shaping needs to be done when measuring text
		FShapedGlyphSequencePtr shaped_text = in_shaped_text_cache->find_or_add_shaped_text(in_run_key, in_text);

		std::optional<int32_t> measured_width = shaped_text->get_measured_width(in_measure_range.m_begin_index, in_measure_range.m_end_index);

		if (!measured_width.has_value())
		{
			FCachedShapedTextKey measure_key = in_run_key;
			measure_key.m_text_range = in_measure_range;

			shaped_text = in_shaped_text_cache->find_or_add_shaped_text(measure_key, in_text);
			measured_width = shaped_text->get_measured_width();
		}

		return glm::vec2(measured_width.value(), shaped_text->get_max_text_height());
	}
	FShapedGlyphSequencePtr FShapedTextCache::find_shaped_text(const FCachedShapedTextKey& in_key) const
	{
		auto it = m_cached_shaped_text.find(in_key);

		if (it != m_cached_shaped_text.end() && !it->second->is_dirty())
		{
			return it->second;
		}

		return nullptr;
	}
	FShapedGlyphSequencePtr FShapedTextCache::find_or_add_shaped_text(const FCachedShapedTextKey& in_key, const DoDoUtf8String& in_text)
	{
		FShapedGlyphSequencePtr shaped_text = find_shaped_text(in_key);

		if (!shaped_text)
		{
			shaped_text = add_shaped_text(in_key, in_text);
		}

		return shaped_text;
	}
	FShapedGlyphSequencePtr FShapedTextCache::add_shaped_text(const FCachedShapedTextKey& in_key, const DoDoUtf8String& in_text)
	{
		const std::shared_ptr<FSlateFontCache> font_cache = m_font_cache_ptr.lock();

		if (!font_cache)
		{
			return std::make_shared<FShapedGlyphSequence>();
		}

		//note:this function is important, this will shape a text sequence
		FShapedGlyphSequencePtr shaped_text = font_cache->shape_bidirectional_text(
			in_text,
			in_key.m_text_range.m_begin_index,
			in_key.m_text_range.len(),
			in_key.m_font_info,
			in_key.m_scale,
			in_key.m_text_content.m_base_direction,
			in_key.m_text_content.m_text_shaping_method
		);

		return add_shaped_text(in_key, shaped_text);
	}
	FShapedGlyphSequencePtr FShapedTextCache::add_shaped_text(const FCachedShapedTextKey& in_key, FShapedGlyphSequencePtr in_shaped_text)
	{
		m_cached_shaped_text.insert({ in_key, in_shaped_text });

		return in_shaped_text;
	}
}


