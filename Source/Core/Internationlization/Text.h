#pragma once

#include "Core/Core.h"

namespace DoDo {
	namespace TextBiDi {
		/*lists the potential reading directions for text*/
		enum class ETextDirection : uint8_t
		{
			/*contains only LTR text - requires simple LTR layout*/
			LeftToRight,
			/*contains only RTL text - requires simple RTL layout*/
			RightToLeft,
			/*contains both LTR and RTL text - requires more complex layout using multiple runs of text*/
			Mixed
		};
	}
}