#include "PreCompileHeader.h"

#include "ICUInternationalization.h"

#include <unicode/locid.h>
#include <unicode/timezone.h>
#include <unicode/uclean.h>
#include <unicode/udata.h>

#include "Core/String/DoDoString.h"

#include "Core/Misc/Paths.h"//FPaths

#include "ICUBreakIterator.h"//FICUBreakIteratorManager depends on it

#include "Internationalization.h"//FInternationalization depends on it

namespace DoDo {
	FICUInternationalization::FICUInternationalization(FInternationalization* const i18n)
		: m_i18n(i18n)
	{
	}
	bool FICUInternationalization::initialize()
	{
		//return false;
		UErrorCode icu_status = U_ZERO_ERROR;

		//u_setMemoryFunctions(nullptr)

		const DoDoUtf8String data_directory_relative_to_content = "Internationalization";

		const DoDoUtf8String potential_data_directories[] =
		{
			FPaths::engine_content_dir() / data_directory_relative_to_content //try engine content directory
		};

		//todo:reset m_icu_data_directory

		for (const DoDoUtf8String& potential_data_directory : potential_data_directories)
		{
			//todo:check directory exists
			if (FPaths::directory_exists(potential_data_directory))
			{
				u_setDataDirectory(potential_data_directory.c_str());

				m_icu_data_directory = potential_data_directory / "icudt64l" / "";//we include the sub-folder here as it prevents icu i/o requests outside of this folder

				break;
			}
		}

		udata_setFileAccess(UDATA_FILES_FIRST, &icu_status);//we always need to load loose icu data files
		
		//todo:add set data file functions
		u_init(&icu_status);

		//todo:regex manager create
		FICUBreakIteratorManager::create();

		//todo:from windows api to get default locale 
		m_i18n->m_default_language = find_or_make_culture("en", EAllowDefaultCultureFallback::Yes);

		return U_SUCCESS(icu_status) ? true : false;
	}
	std::shared_ptr<FCulture> FICUInternationalization::find_or_make_culture(const DoDoUtf8String& name, const EAllowDefaultCultureFallback allow_default_fallback)
	{
		//return std::shared_ptr<FCulture>();

		//find the cached culture
		{
			auto it = m_cached_cultures.find(name);
			if (it != m_cached_cultures.end())
			{
				return it->second;
			}
		}

		//if no cached culture is found, try to make one
		std::shared_ptr<FCulture> new_culture;

		UErrorCode icu_status = U_ZERO_ERROR;

		if (UResourceBundle* icu_resource_bundle = ures_open(nullptr, name.c_str(), &icu_status))
		{
			if (icu_status != U_USING_DEFAULT_WARNING || allow_default_fallback == EAllowDefaultCultureFallback::Yes)
			{
				new_culture = FCulture::create(std::make_unique<FICUCultureImplementation>(name));
			}
			ures_close(icu_resource_bundle);
		}

		return new_culture;
	}
}