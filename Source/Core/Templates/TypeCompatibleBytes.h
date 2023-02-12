#pragma once

#include "Core/Core.h"


template<int32_t Size, uint32_t Alignment>
struct TAlignedBytes;

template<int32_t Size>
struct TAlignedBytes<Size, 1>
{
	uint8_t m_pad[Size];
};

/*a macro that implements TAlignedBytes for a specific alignment*/
#define IMPLEMENT_ALIGNED_STORAGE(Align) \
	template<int32_t Size> \
	struct TAlignedBytes<Size, Align> \
	{ \
		struct alignas(Align) TPadding \
		{ \
			uint8_t Pad[Size]; \
		}; \
		TPadding m_padding;\
	};

IMPLEMENT_ALIGNED_STORAGE(16);//note:align for 16 bytes

#undef IMPLEMENT_ALIGNED_STORAGE
