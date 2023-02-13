#pragma once

#include "SlateCore/Rendering/SlateLayoutTransform.h"

namespace DoDo
{
	class FLayoutGeometry//useful for SSplitter
	{
	public:
		FLayoutGeometry()
			: m_local_size(1.0f, 1.0f)
		{}

		explicit FLayoutGeometry(const FSlateLayoutTransform& in_local_to_parent, const glm::vec2& size_in_local_space)
			: m_local_to_parent(in_local_to_parent)
			, m_local_size(size_in_local_space)
		{}

		const FSlateLayoutTransform& get_local_to_parent_transform() const
		{
			return m_local_to_parent;
		}

		glm::vec2 get_size_in_local_space() const
		{
			return m_local_size;
		}

	private:
		FSlateLayoutTransform m_local_to_parent;

		glm::vec2 m_local_size;
	};
}
