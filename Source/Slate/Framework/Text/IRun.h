#pragma once

#include "Core/Templates/TypeHash.h"

#include "Core/Internationlization/Text.h"//TextBiDi::ETextDirection depends on it

#include "Core/String/DoDoString.h"

#include "Slate/Framework/Text/ShapedTextCacheFwd.h"

namespace DoDo {

	enum class ETextShapingMethod : uint8_t;

	struct FRunInfo
	{
		FRunInfo()
			: m_name()
			, m_meta_data()
		{

		}

		FRunInfo(DoDoUtf8String in_name)
			: m_name(in_name)
			, m_meta_data()
		{

		}

		DoDoUtf8String m_name;

		std::map<DoDoUtf8String, DoDoUtf8String> m_meta_data;
	};

	/*the basic data needed when shaping a run of text*/
	struct FShapedTextContext
	{
		FShapedTextContext(const ETextShapingMethod in_text_shaping_method, const TextBiDi::ETextDirection in_base_direction)
			: m_text_shaping_method(in_text_shaping_method)
			, m_base_direction(in_base_direction)
		{}

		bool operator==(const FShapedTextContext& other) const
		{
			return m_text_shaping_method == other.m_text_shaping_method
				&& m_base_direction == other.m_base_direction;
		}

		bool operator!=(const FShapedTextContext& other) const
		{
			return !(*this == other);
		}

		friend inline uint32_t Get_Type_Hash(const FShapedTextContext& key)
		{
			uint32_t key_hash = 0;
			key_hash = Hash_Combine(key_hash, Get_Type_Hash(key.m_text_shaping_method));
			key_hash = Hash_Combine(key_hash, Get_Type_Hash(key.m_base_direction));
			return key_hash;
		}

		/*the method used to shape the text within this layout*/
		ETextShapingMethod m_text_shaping_method;

		/*the base direction of the current line of text*/
		TextBiDi::ETextDirection m_base_direction;
	};

	/*the context needed when performing text operations on a run of text*/
	/*a map of FShapedGlyphSequence*/
	struct FRunTextContext : public FShapedTextContext
	{
		FRunTextContext(const ETextShapingMethod in_text_shaping_method, const TextBiDi::ETextDirection in_base_direction, FShapedTextCachePtr in_shaped_text_cache)
			: FShapedTextContext(in_text_shaping_method, in_base_direction)
			, m_shaped_text_cache(in_shaped_text_cache)
		{}
		/*the shaped text cache that should be used by this line of text*/
		FShapedTextCachePtr m_shaped_text_cache;
	};

	struct FLayoutBlockTextContext : public FRunTextContext
	{
		FLayoutBlockTextContext(const FRunTextContext& in_run_text_context, const TextBiDi::ETextDirection in_text_direction)
			: FRunTextContext(in_run_text_context)
			, m_text_direction(in_text_direction)
		{}

		bool operator==(const FLayoutBlockTextContext& other) const
		{
			return FRunTextContext::operator==(other)
				&& m_text_direction == other.m_text_direction;
		}

		bool operator!=(const FLayoutBlockTextContext& other) const
		{
			return !(*this == other);
		}

		friend inline uint32_t Get_Type_Hash(const FLayoutBlockTextContext& key)
		{
			uint32_t key_hash = Get_Type_Hash(static_cast<const FRunTextContext&>(key));
			key_hash = Hash_Combine(key_hash, Get_Type_Hash(key.m_text_direction));
			return key_hash;
		}
		/*the reading direction of the text contained within this block*/
		TextBiDi::ETextDirection m_text_direction;
	};
	struct FTextRange;
	class ILayoutBlock;
	class IRun
	{
	public:

		virtual ~IRun() {}

		virtual std::shared_ptr<ILayoutBlock> create_block(int32_t start_index, int32_t end_index, glm::vec2 size, const FLayoutBlockTextContext& text_context) = 0;

		virtual FTextRange get_text_range() const = 0;
		virtual int16_t get_base_line(float scale) const = 0;
		virtual int16_t get_max_height(float scale) const = 0;
		virtual void set_text_range(const FTextRange& value) = 0;

		virtual glm::vec2 measure(int32_t start_index, int32_t end_index, float scale, const FRunTextContext& text_context) const = 0;
	};
}