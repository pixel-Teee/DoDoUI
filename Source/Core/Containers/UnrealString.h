#pragma once

#include "Core/Core.h"

#include "Core/Templates/TypeHash.h"

namespace DoDo {
	class DoDoUtf8String;
	struct FTextRange
	{
		FTextRange()
			: m_begin_index(-1)
			, m_end_index(-1)
		{}

		FTextRange(int32_t in_begin_index, int32_t in_end_index)
			: m_begin_index(in_begin_index)
			, m_end_index(in_end_index)
		{}

		bool operator==(const FTextRange& other) const
		{
			return m_begin_index == other.m_begin_index
				&& m_end_index == other.m_end_index;
		}

		friend inline uint32_t Get_Type_Hash(const FTextRange& key)
		{
			uint32_t key_hash = 0;
			key_hash = Hash_Combine(key_hash, Get_Type_Hash(key.m_begin_index));
			key_hash = Hash_Combine(key_hash, Get_Type_Hash(key.m_end_index));
			return key_hash;
		}

		int32_t len() const { return m_end_index - m_begin_index; }

		int32_t m_begin_index;
		int32_t m_end_index;

		/*
		* produce an array of line ranges from the given text, breaking at any new-line characters
		*/
		static void calculate_line_ranges_from_string(const DoDoUtf8String& input, std::vector<FTextRange>& line_ranges);
	};
}