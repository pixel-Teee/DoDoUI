#pragma once

#include "Core/Core.h"

namespace DoDo
{
	inline uint32_t Get_Type_Hash(const uint64_t A)
	{
		return (uint32_t)A + ((uint32_t)(A >> 32) * 23);
	}
}