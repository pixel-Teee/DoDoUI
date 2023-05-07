#pragma once

#include "Core/Core.h"

#include "Core/Templates/TypeHash.h"

namespace DoDo {
	struct FTextRange
	{

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
	};
}