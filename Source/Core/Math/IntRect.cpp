#include "PreCompileHeader.h"

#include "IntRect.h"

#include "Box2D.h"

namespace DoDo {
	FIntRect::FIntRect()
		: m_min(EForceInit::ForceInit)
		, m_max(EForceInit::ForceInit)
	{
	}
	FIntRect::FIntRect(int32_t x0, int32_t y0, int32_t x1, int32_t y1)
		: m_min(x0, y0)
		, m_max(x1, y1)
	{
	}
	int32_t FIntRect::Area() const
	{
		return (m_max.x - m_min.x) * (m_max.y - m_min.y);
	}
}