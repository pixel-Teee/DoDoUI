#include <PreCompileHeader.h>

#include "Geometry.h"

#include "ArrangedWidget.h"

#include "SlateCore/Widgets/SWidget.h"

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
			std::memcpy(this, &rhs, sizeof(FGeometry));
		}
		return *this;
	}

	FArrangedWidget FGeometry::make_child(const std::shared_ptr<SWidget>& child_widget, const glm::vec2& in_local_size,
		const FSlateLayoutTransform& layout_transform) const
	{
		//todo:implement this function
		const std::optional<FSlateRenderTransform> render_transform = child_widget->get_render_transform_with_respect_to_flow_direction();

		if(render_transform.has_value())
		{
			//todo:implement get render transform pivot with respect to flow direction
			const glm::vec2 render_transform_pivot = child_widget->get_render_transform_pivot_with_respect_to_flow_direction();
			return FArrangedWidget(child_widget, make_child(in_local_size, layout_transform, render_transform.value(), render_transform_pivot));
		}
		else
		{
			return FArrangedWidget(child_widget, make_child(in_local_size, layout_transform));
		}
	}

	FArrangedWidget FGeometry::make_child(const std::shared_ptr<SWidget>& child_widget, const glm::vec2& child_offset,
	                                      const glm::vec2& in_local_size, float child_scale) const
	{
		return make_child(child_widget, in_local_size, FSlateLayoutTransform(child_scale, DoDo::transform_point(child_scale, child_offset)));
	}
}
