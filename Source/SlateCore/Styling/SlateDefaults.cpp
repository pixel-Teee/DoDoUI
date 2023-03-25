#include "PreCompileHeader.h"

#include "SlateDefaults.h"

#include "SlateCore/Brushes/SlateNoResource.h"//FSlateNoResource depends on it

namespace DoDo {
	const FSlateBrush* FStyleDefaults::get_no_brush()
	{
		static const FSlateNoResource no_brush;

		return &no_brush;
	}
}