#pragma once

#include "Core/Core.h"

namespace DoDo
{
	inline uint32_t Hash_Combine(uint32_t A, uint32_t C)
	{
		uint32_t B = 0x9e3779b9;
		A += B;

		A -= B; A -= C; A ^= (C >> 13);
		B -= C; B -= A; B ^= (A << 8);
		C -= A; C -= B; C ^= (B >> 13);
		A -= B; A -= C; A ^= (C >> 12);
		B -= C; B -= A; B ^= (A << 16);
		C -= A; C -= B; C ^= (B >> 5);
		A -= B; A -= C; A ^= (C >> 3);
		B -= C; B -= A; B ^= (A << 10);
		C -= A; C -= B; C ^= (B >> 15);

		return C;
	}
	inline uint32_t Get_Type_Hash(const uint8_t A)
	{
		return A;
	}
	inline uint32_t Get_Type_Hash(const int8_t A)
	{
		return A;
	}
	inline uint32_t Get_Type_Hash(const uint16_t A)
	{
		return A;
	}
	inline uint32_t Get_Type_Hash(const int16_t A)
	{
		return A;
	}
	inline uint32_t Get_Type_Hash(const int32_t A)
	{
		return A;
	}
	inline uint32_t Get_Type_Hash(const uint32_t A)
	{
		return A;
	}
	inline uint32_t Get_Type_Hash(const uint64_t A)
	{
		return (uint32_t)A + ((uint32_t)(A >> 32) * 23);
	}
	inline uint32_t Get_Type_Hash(const int64_t A)
	{
		return (uint32_t)A + ((uint32_t)(A >> 32) * 23);
	}

	inline uint32_t Get_Type_Hash(float Value)
	{
		return *(uint32_t*)&Value;
	}
}