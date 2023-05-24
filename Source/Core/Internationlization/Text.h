#pragma once

#include "Core/Core.h"

namespace DoDo {
	class DoDoUtf8String;
	namespace TextBiDi {
		/*lists the potential reading directions for text*/
		enum class ETextDirection : uint8_t
		{
			/*contains only LTR text - requires simple LTR layout*/
			LeftToRight,
			/*contains only RTL text - requires simple RTL layout*/
			RightToLeft,
			/*contains both LTR and RTL text - requires more complex layout using multiple runs of text*/
			Mixed
		};

		/*a single complex layout entry, defines the starting position, length, and reading direction for a sub-section of text*/
		struct FTextDirectionInfo
		{
			int32_t m_start_index;
			int32_t m_length;
			ETextDirection m_text_direction;
		};

		/*defines the interface for a re-usable bidi object*/
		class ITextBiDi
		{
		public:
			virtual ~ITextBiDi() {}

			virtual ETextDirection compute_text_direction(const DoDoUtf8String& in_string, const int32_t in_string_start_index, const int32_t in_string_len, const ETextDirection in_base_direction,
				std::vector<FTextDirectionInfo>& out_text_direction_info) = 0;
		};

		/*
		* creates a re-usable bidi object
		* this may yidel better perfromance than the utility functions if you're performing a lot of bidi requests, as this object can re-use allocated data between requests
		*/
		std::unique_ptr<ITextBiDi> create_text_bidi();
	}

}