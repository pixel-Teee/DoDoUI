#pragma once

//------unicode------
#include <unicode/umachine.h>

#include <unicode/gregocal.h>
//------unicode------

#include "Core/String/DoDoString.h"

namespace DoDo {
	//FImplementation
	class FInternationalization;
	class FCulture;
	class FICUInternationalization
	{
	public:
		FICUInternationalization(FInternationalization* const i18n);

		bool initialize();

		enum class EAllowDefaultCultureFallback : uint8_t { No, Yes };
		std::shared_ptr<FCulture> find_or_make_culture(const DoDoUtf8String& name, const EAllowDefaultCultureFallback allow_default_fallback);
	private:
		FInternationalization* const m_i18n;

		DoDoUtf8String m_icu_data_directory;

		std::map<DoDoUtf8String, std::shared_ptr<FCulture>> m_cached_cultures;
	};
}