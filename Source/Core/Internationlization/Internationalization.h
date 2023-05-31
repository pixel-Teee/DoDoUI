#pragma once

#include "ICUInternationalization.h"//FICUInternationalization depends on it

namespace DoDo {
	class FInternationalization
	{
	public:
		static FInternationalization& get();

		bool is_initialized() const { return m_b_is_initialized; }
	private:
		void initialize();

		typedef FICUInternationalization FImplementation;
		std::unique_ptr<FImplementation> m_implementation;

		bool m_b_is_initialized = false;
	};
}