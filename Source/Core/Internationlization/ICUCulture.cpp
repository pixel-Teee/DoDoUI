#include "PreCompileHeader.h"

#include "ICUCulture.h"

#include "Core/String/DoDoString.h"

#include "ICUUtilities.h"

namespace DoDo {
	namespace
	{
		const icu::Locale& GetInvariantLocale()
		{
			auto make_invariant_locale = []()
			{
				icu::Locale tmp_locale("en-US-POSIX");
				if (tmp_locale.isBogus())
				{
					tmp_locale = icu::Locale();
				}
				return tmp_locale;
			};

			static const icu::Locale invariant_locale = make_invariant_locale();

			return invariant_locale;
		}

		std::shared_ptr<const icu::BreakIterator> create_break_iterator(const icu::Locale& icu_locale, const EBreakIteratorType type)
		{
			UErrorCode icu_status = U_ZERO_ERROR;

			icu::BreakIterator* (*FactoryFunction)(const icu::Locale&, UErrorCode&) = nullptr;

			switch (type)
			{
			case EBreakIteratorType::Grapheme:
				FactoryFunction = icu::BreakIterator::createCharacterInstance;
				break;
			}
			std::shared_ptr<const icu::BreakIterator> ptr = std::shared_ptr<const icu::BreakIterator>(FactoryFunction(icu_locale, icu_status));

			//todo:check error

			return ptr;
		}
	}
	FICUCultureImplementation::FICUCultureImplementation(const DoDoUtf8String& locale_name)
		: m_icu_locale(locale_name.c_str()) //utf8->ansi
	{
		if (m_icu_locale.isBogus())
		{
			m_icu_locale = GetInvariantLocale();
		}

		{
			UErrorCode icu_status = U_ZERO_ERROR;
			m_icu_cardinal_plural_rules = icu::PluralRules::forLocale(m_icu_locale, UPLURAL_TYPE_CARDINAL, icu_status);
		}

		{
			//todo:add more logic
		}
	}
	std::shared_ptr<const icu::BreakIterator> FICUCultureImplementation::get_break_iterator(const EBreakIteratorType type)
	{
		std::shared_ptr<const icu::BreakIterator> result;

		switch (type)
		{
		case EBreakIteratorType::Grapheme:
			{
				result = m_icu_grapheme_break_iterator ? m_icu_grapheme_break_iterator : (m_icu_grapheme_break_iterator = create_break_iterator(m_icu_locale, type));
			}
			break;
		}

		return result;
	}
}