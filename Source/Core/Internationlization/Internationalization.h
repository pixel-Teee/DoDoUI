#pragma once

#include "ICUInternationalization.h"//FICUInternationalization depends on it

#include "Culture.h"//FCulture

namespace DoDo {
	class FInternationalization
	{
	public:
		friend class FICUInternationalization;

		static FInternationalization& get();

		bool is_initialized() const { return m_b_is_initialized; }

		/*get the default culture specified by the os
		* @note this function exists for legacy api parity with get current culture and is equivalent to get default language, it should never be used in internal localization/internationalization code!
		*/
		std::shared_ptr<FCulture> get_default_culture() const
		{
			return m_default_language;
		}

		FInternationalization();

		~FInternationalization();
	private:
		void initialize();

		typedef FICUInternationalization FImplementation;
		std::unique_ptr<FImplementation> m_implementation;

		bool m_b_is_initialized = false;

		/*
		* the default language specified by the os
		*/
		std::shared_ptr<FCulture> m_default_language;
	};
}