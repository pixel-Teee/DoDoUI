#pragma once

#include <glm/vec2.hpp>
#include "SlateCore/Rendering/SlateRenderTransform.h"//accumulated render transform depends on it

/*
* a paint geometry contains the window-space (draw-space) info to draw an element on the screen
* it contains the size of the element in the element's local space along with the transform needed to position the element in window space
* 
* DrawPosition/DrawSize/DrawScale are maintained for legacy reasons, but are deprecated:
* 
* The DrawPosition and DrawSize are already positioned and scaled for immediate consumption by the draw code
* 
* The DrawScale is only applied to the internal aspects of the draw primitives
* e.g. Line thickness, 3x3 grid margins, etc.
*/

namespace DoDo
{
	class FSlateLayoutTransform;//forward declare

	struct FPaintGeometry
	{
		/*
		* deprecated
		* drawing should only happen in local space to ensure render transforms work
		* warning : this legacy member represents is layout space, and does not account for render transforms
		* 
		* render-space position at which we will draw
		*/
		glm::vec2 m_draw_position;

		/*
		* deprecated
		* drawing should only happen in local space to ensure render transforms work
		* warning : this legacy member represents is layout space, and does not account for render transforms
		* 
		* only affects the draw aspects of individual draw elements. e.g. link thickness, 3x3 grid margins
		*/
		float m_draw_scale;

		/* get the size of the geometry in local space, must call commit transforms if using legacy constructor() first if legacy ctor is used */
		const glm::vec2& get_local_Size() const { return m_local_size; }

	private:
		/*mutable to support legacy constructors, doesn't account for render transforms*/
		mutable glm::vec2 m_draw_size;

		/*mutable to support legacy constructors*/
		mutable glm::vec2 m_local_size;

		/*
		* final render transform for drawing, transform from local space to window space for the draw element
		* mutable to support legacy constructors
		*/
		mutable FSlateRenderTransform m_accumulated_render_transform;

		//support legacy constructors
		uint8_t m_b_using_legacy_constructor : 1;

		uint8_t m_b_has_render_transform : 1;

	public:
		/*default ctor*/
		FPaintGeometry()
			: m_draw_position(0.0f, 0.0f)
			, m_draw_scale(1.0f)
			, m_draw_size(0.0f, 0.0f)
			, m_local_size(0.0f, 0.0f)
			, m_accumulated_render_transform()
			, m_b_using_legacy_constructor(true)
			, m_b_has_render_transform(false)
		{

		}

		/*deprecated, this is legacy and should be removed*/
		FPaintGeometry(glm::vec2 in_draw_position, glm::vec2 in_draw_size, float in_draw_scale)
			: m_draw_position(in_draw_position)
			, m_draw_scale(in_draw_scale)
			, m_draw_size(in_draw_size)
			, m_local_size(0.0f, 0.0f)
			, m_accumulated_render_transform()
			, m_b_using_legacy_constructor(true)
			, m_b_has_render_transform(false)
		{

		}

		/*
		* special case method to append a layout transform to paint geometry
		* this is used in cases where the FGeometry was arranged in desktop space
		* and we need to undo the root desktop translation to get into window space
		* if you find yourself wanting to use this function, ask someone if there's a better way
		* 
		* @param LayoutTransform an additional layout transform to append to this paint geometry
		*/
		void append_transform(const FSlateLayoutTransform& layout_transform)
		{

		}

		/*
		* special case method to replace the render transform on a paint geometry
		* 
		* @param RenderTransform an additional layout transform to append to this paint geometry
		*/
		void set_render_transform(const FSlateRenderTransform& render_transform)
		{
			m_accumulated_render_transform = render_transform;
		}
	};
}