#include "PreCompileHeader.h"

#include "Text.h"

namespace DoDo {
	std::unique_ptr<TextBiDi::ITextBiDi> TextBiDi::create_text_bidi()
	{
		return std::unique_ptr<ITextBiDi>();
	}
}


