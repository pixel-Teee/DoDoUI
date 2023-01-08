#pragma once
#include "SlateCore/Rendering/SlateRenderTransform.h"//FSlateRenderTransform depends on it

namespace DoDo
{
	/*
	 * represents the position, size, and absolute of a widget in slate
	 * the absolute location of a geometry is usually screen space or window space depending on where the geometry originated
	 * 
	 * geometries are usually paired with a SWidget pointer in order to provide information about a specific widget(see FArrangedWidget)
	 * a geometry's parent is generally thought to be the geometry of the corresponding parent widget
	 */
	struct FGeometry
	{
	public:
		/*
		 * default constructor
		 * creates a geometry with identity transforms
		 */
		FGeometry();

		/*
		 * hack, we're keeping members of FGeometry const to prevent mutability without making them private, for backward compatibility
		 * but this means the assignment operator no longer works, we implement one ourselves now and force a memcpy
		 */
		FGeometry& operator=(const FGeometry& rhs);

		/*
		 * deprecated function, use make child taking a layout transform instead!
		 * construct a new geometry given the following parameters:
		 *
		 * @param OffsetFromParent local position of this geometry within it's parent geometry
		 * @param ParentAbsolutePosition the absolute position of the parent geometry containing this geometry
		 * @param InSize the size of this geometry
		 * @param InScale the scale of this geometry with respect to Normal Slate Coordinates
		 */

	private:

		/* return the size of the geometry in local space*/
		const glm::vec2& get_local_size() const { return m_size; }

		/*
		 * get the local position on the surface of the geometry using normalized coordinates
		 * (0, 0) - upper left
		 * (1, 1) - bottom right
		 *
		 * example : say you wanted to know the center of the widget in local space, GetLocalPositionAtCoordinates(FVector2D(0.5f, 0.5f));
		 */

		glm::vec2 get_local_position_at_coordinates(const glm::vec2& normal_coordinates) const
		{
			return glm::vec2(m_local_position) + normal_coordinates * get_local_size();
		}

		bool has_render_transform() const { return m_b_has_render_transform; }

	public:
		const glm::vec2 m_size;

		const float m_scale;

		const glm::vec2 m_absolute_position;

		/*
		 * deprecated
		 *
		 * position of the geometry with respect to it's parent in local space, equivalent to the translation portion of the Local->Parent layout transform
		 * if you know your children have no additional scale applied to them, you can use this as the local->Parent layout transform, if your children
		 * do have additional scale applied, there is no way to determine the actual local->Parent layout transform, since the scale is accumulated
		 */
		const glm::vec2 m_local_position;
	private:
		/*
		 * concatenated render transform, actual transform used for rendering
		 * formed as : Concat(LocalRenderTransform, LocalLayoutTransform, Parent->AccumulatedRenderTransform)
		 *
		 * for rendering, absolute coordinates will always be in window space(relative to the root window)
		 */
		FSlateRenderTransform m_accumulated_render_transform;

		const uint8_t m_b_has_render_transform : 1;
	};


	//todo:fix this 
	//template<> struct TIsPODType<FGeometry> { enum {Value = true};};

}
