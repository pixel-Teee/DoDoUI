#pragma once

#include "CultureImplementation.h"//ICultureImplementation depends on it

#include <unicode/locid.h>
#include <unicode/brkiter.h>
#include <unicode/coll.h>
#include <unicode/numfmt.h>
#include <unicode/decimfmt.h>
#include <unicode/datefmt.h>
#include <unicode/plurrule.h>

namespace DoDo {
	class DoDoUtf8String;

	enum class EBreakIteratorType
	{
		Grapheme,
		Word,
		Line,
		Sentence,
		Title
	};
	class FICUCultureImplementation : public ICultureImplementation
	{
		friend class FCulture;
		friend class FICUBreakIteratorManager;

	public:
		FICUCultureImplementation(const DoDoUtf8String& locale_name);

		virtual ~FICUCultureImplementation() = default;

		std::shared_ptr<const icu::BreakIterator> get_break_iterator(const EBreakIteratorType type);

	private:
		icu::Locale m_icu_locale;

		const icu::PluralRules* m_icu_cardinal_plural_rules;//one
		const icu::PluralRules* m_icu_ordinal_plural_rules;//first

		std::shared_ptr<const icu::BreakIterator> m_icu_grapheme_break_iterator;
	};
}