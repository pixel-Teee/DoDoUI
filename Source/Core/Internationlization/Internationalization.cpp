#include "PreCompileHeader.h"

#include "Internationalization.h"

#include "Core/Misc/LazySingleton.h"//TLazySingleton

namespace DoDo {
	FInternationalization& FInternationalization::get()
	{
		FInternationalization& singleton = TLazySingleton<FInternationalization>::Get();
		singleton.initialize();
		return singleton;
	}
	FInternationalization::FInternationalization()
		: m_implementation(std::make_unique<FImplementation>(this))
	{
	}
	FInternationalization::~FInternationalization()
	{
	}
	void FInternationalization::initialize()
	{
		static bool is_initializing = false;

		if (is_initialized() || is_initializing)
		{
			return;
		}

		struct FInitializingGuard
		{
			FInitializingGuard() { is_initializing = true; }
			~FInitializingGuard() { is_initializing = false; }
		} initializing_guard;

		m_b_is_initialized = m_implementation->initialize();
	}
}