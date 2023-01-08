#include <PreCompileHeader.h>

#include "Geometry.h"

namespace DoDo
{
	FGeometry::FGeometry()
		: m_size(0.0f, 0.0f)
		, m_scale(1.0f)
		, m_absolute_position(0.0f, 0.0f)
		, m_local_position(0.0f)
		, m_b_has_render_transform(false)
	{
	}

	FGeometry& FGeometry::operator=(const FGeometry& rhs)
	{
		//hack to allow use to make FGeometry public members immutable to catch misure
		if(this != &rhs)
		{
			//todo:implement memcpy rhs to lhs
		}
		return *this;
	}
}
