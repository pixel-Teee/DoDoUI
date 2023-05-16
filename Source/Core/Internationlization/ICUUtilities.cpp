#include "PreCompileHeader.h"

#include "ICUUtilities.h"

#include "unicode/ucnv.h"

#include "Core/String/DoDoString.h"//DoDoUtf8String depends on it

namespace DoDo {
	namespace ICUUtilities
	{
		void FStringConverter::convert_string(const DoDoUtf8String& source, const int32_t source_start_index, const int32_t source_len, icu::UnicodeString& destination, const bool should_null_terminate)
		{
			m_impl.convert_string(source, 0, source.get_length(), destination, should_null_terminate);
		}
		FStringConverterImpl_ConvertToUnicodeString::FStringConverterImpl_ConvertToUnicodeString()
		{
		}
		FStringConverterImpl_ConvertToUnicodeString::~FStringConverterImpl_ConvertToUnicodeString()
		{
		}
		void FStringConverterImpl_ConvertToUnicodeString::convert_string(const DoDoUtf8String& source, const int32_t source_start_index, const int32_t source_len, icu::UnicodeString& destination, const bool should_null_terminate)
		{
			if (source_len > 0)
			{
				UErrorCode icu_status = U_ZERO_ERROR;

				ucnv_reset(m_icu_converter);

				//get the internal buffer of the string, we're going to use it as scratch space
				const int32_t destionation_capacity_u_chars = source_len * 2;
				UChar* internal_string_buffer = destination.getBuffer(destionation_capacity_u_chars);

				//perform the conversion into the string buffer
				const int32_t souce_size_bytes = source.get_count();
				const int32_t destination_length = ucnv_toUChars(m_icu_converter, internal_string_buffer, destionation_capacity_u_chars,
					source.c_str() + source_start_index, souce_size_bytes, &icu_status);

				//optionally null terminate the string
				if (should_null_terminate)
				{
					internal_string_buffer[destination_length] = 0;
				}

				//size it back down to the correct size and release our lock on the string buffer
				destination.releaseBuffer(destination_length);

				U_SUCCESS(icu_status);
			}
			else
			{
				destination.remove();
			}
		}
	}
}

