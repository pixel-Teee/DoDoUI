#include "PreCompileHeader.h"

#include "Text.h"

#include "ICUUtilities.h"//ICUUtilities::FStringConverter depends on it

//------icu------
#include "unicode/utypes.h"
#include "unicode/unistr.h"
#include "unicode/coll.h"
#include "unicode/sortkey.h"
#include "unicode/ubidi.h"
//------icu------

#include "Core/String/DoDoString.h"//icu

namespace DoDo {
	namespace TextBiDi
	{
		namespace Internal
		{
			ETextDirection icu_to_ue(const UBiDiDirection in_direction)
			{
				switch (in_direction)
				{
				case UBIDI_LTR:
					return ETextDirection::LeftToRight;
				case UBIDI_RTL:
					return ETextDirection::RightToLeft;
				case UBIDI_MIXED:
					return ETextDirection::Mixed;
				default:
					break;
				}
			}

			UBiDiLevel get_paragraph_direction(const ETextDirection in_base_direction)
			{
				return (in_base_direction == ETextDirection::LeftToRight) ? 0 : 1;//0 = LTR, 1 = RTL
			}

			ETextDirection compute_text_direction(UBiDi* in_icu_bidi, const icu::UnicodeString& in_icu_string, const int32_t in_string_offset, const ETextDirection in_base_direction,
				std::vector<FTextDirectionInfo>& out_text_direction_info)
			{
				UErrorCode icu_status = U_ZERO_ERROR;

				//todo:fix me
				ubidi_setPara(in_icu_bidi, in_icu_string.getBuffer(), in_icu_string.length(), get_paragraph_direction(ETextDirection::LeftToRight), nullptr, &icu_status);

				if (U_SUCCESS(icu_status))
				{
					//return Internal::icu_to_ue(ubidi_getDirection(in_icu_bidi));

					const ETextDirection return_direction = Internal::icu_to_ue(ubidi_getDirection(in_icu_bidi));

					const int32_t run_count = ubidi_countRuns(in_icu_bidi, &icu_status);

					out_text_direction_info.resize(run_count);

					for (int32_t run_iundex = 0; run_iundex < run_count; ++run_iundex)
					{
						FTextDirectionInfo& cur_text_direction_info = out_text_direction_info[run_iundex];

						int32_t internal_start_index = 0;
						int32_t internal_length = 0;
						cur_text_direction_info.m_text_direction = Internal::icu_to_ue(ubidi_getVisualRun(in_icu_bidi, run_iundex, &internal_start_index, &internal_length));

						//adjust the index and length for what FString expects (icu always uses utf-16 indices internally, and FString might not be utf-16)
						cur_text_direction_info.m_start_index = in_string_offset + internal_start_index;//todo:add get native string length
						cur_text_direction_info.m_length = internal_length;
					}

					return return_direction;
				}
				else
				{
					//check error
				}

				return ETextDirection::LeftToRight;
			}
			//icu class
			class FICUTextBiDi : public ITextBiDi
			{
			public:
				FICUTextBiDi()
					: m_IcuBiDi(ubidi_open())
				{}

				~FICUTextBiDi()
				{
					ubidi_close(m_IcuBiDi);
					m_IcuBiDi = nullptr;
				}

				virtual ETextDirection compute_text_direction(const DoDoUtf8String& in_string, 
				const int32_t in_string_start_index, const int32_t in_string_len, const ETextDirection in_base_direction, std::vector<FTextDirectionInfo>& out_text_direction_info) override
				{
					out_text_direction_info.clear();

					if (in_string_len == 0)
					{
						return ETextDirection::LeftToRight;
					}

					//DoDoUtf8String convert to icu string
					m_string_converter.convert_string(in_string, in_string_start_index, in_string_len, m_icu_string);

					return Internal::compute_text_direction(m_IcuBiDi, m_icu_string, in_string_start_index, in_base_direction, out_text_direction_info);
				}

			private:
				/*non copyable*/
				FICUTextBiDi(const FICUTextBiDi&);
				FICUTextBiDi& operator=(const FICUTextBiDi&);

				UBiDi* m_IcuBiDi;
				icu::UnicodeString m_icu_string;
				ICUUtilities::FStringConverter m_string_converter;
			};
		}
		std::unique_ptr<TextBiDi::ITextBiDi> TextBiDi::create_text_bidi()
		{
			return std::make_unique<Internal::FICUTextBiDi>();
		}
	}
	
	bool DoDoUtf8String::is_white_space(const DoDoUtf8String& Char)
	{
		//tchar should either be utf-16 or utf-32, so we should be fine to cast it to a UChar32 for the whitespace
		//check, since whitespace is never a pair of utf-16 characters
		const UChar32 icu_char = const_cast<DoDoUtf8String&>(Char).to_utf16_code_point(0);
		return u_isWhitespace(icu_char) != 0;
	}
}