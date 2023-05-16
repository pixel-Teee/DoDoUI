#include "PreCompileHeader.h"

#include "Text.h"

//------icu------
#include "unicode/utypes.h"
#include "unicode/unistr.h"
#include "unicode/coll.h"
#include "unicode/sortkey.h"
#include "unicode/ubidi.h"
//------icu------

namespace DoDo {
	namespace TextBiDi
	{
		namespace Internal
		{
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

			private:
				/*non copyable*/
				FICUTextBiDi(const FICUTextBiDi&);
				FICUTextBiDi& operator=(const FICUTextBiDi&);

				UBiDi* m_IcuBiDi;
				icu::UnicodeString m_icu_string;
				//ICUUtilities::FStringConverter
			};
		}
		std::unique_ptr<TextBiDi::ITextBiDi> TextBiDi::create_text_bidi()
		{
			return std::unique_ptr<Internal::FICUTextBiDi>();
		}
	}
	
}