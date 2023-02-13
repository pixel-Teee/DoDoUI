#include <PreCompileHeader.h>

#include "SlateLayoutTransform.h"

namespace DoDo {
	FSlateLayoutTransform::FSlateLayoutTransform(float in_scale, const glm::vec2& in_translation)
		: m_scale(in_scale)
		, m_translation(in_translation)
	{
	}
}
