#pragma once

#include "Core/Core.h"

namespace DoDo {
	struct FCompositeFont;
	/*cache used to efficiently upgrade legacy FSlateFontInfo structs to use a composite font by reducing the amount of duplicate instances that are created*/
	class FLegacySlateFontInfoCache : public std::enable_shared_from_this<FLegacySlateFontInfoCache>
	{
	public:
		static FLegacySlateFontInfoCache& get();
		/*
		* get or create an appropriate composite font from the legacy font name
		*/
		std::shared_ptr<const FCompositeFont> get_default_font();

	private:
		std::shared_ptr<const FCompositeFont> m_default_font;

		static std::shared_ptr<FLegacySlateFontInfoCache> m_singleton;
	};
}