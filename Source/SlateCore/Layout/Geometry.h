#pragma once
//#include "ArrangedWidget.h"//FArrangedWidget
#include "PaintGeometry.h"//FPaintGeometry depends on it
#include "SlateCore/Rendering/SlateRenderTransform.h"//FSlateRenderTransform depends on it

#include "SlateRect.h"//FSlateRect
#include "SlateRotatedRect.h"//FSlateRotatedRect

namespace DoDo
{
	class FLayoutGeometry;
	class FArrangedWidget;
	class SWidget;
	/*
	 * represents the position, size, and absolute position of a widget in slate
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
		FGeometry(const glm::vec2& offset_from_parent, const glm::vec2& parent_absolute_position, const glm::vec2& in_local_size, float in_scale)
			: m_size(in_local_size)
			, m_scale(1.0f)//this is just a normal value, is in [0.0f, 1.0f]
			, m_absolute_position(0.0f, 0.0f)
			, m_local_position(0.0f, 0.0f)//don't use this
			, m_b_has_render_transform(false)
		{
			//since offset from parent is given as a local space offset, we must convert this offset into the space of the parent to construct a valid layout transform

		}
	private:
		/*
		 * construct a new geometry with a given size in local space that is attached to a parent geometry with the given layout and render transform
		 *
		 * @param InLocalSize the size of the geometry in local size
		 * @param InLocalLayoutTransform a layout transform from local space to the parent geometry's local space
		 * @param InLocalRenderTransform a render-only transform in local space that will be prepended to the LocalLayoutTransform when rendering
		 * @param InLocalRenderTransformPivot pivot in normalizes local space for the local render transform
		 * @param ParentAccumulatedLayoutTransform the accumulated layout transform of the parent widget, AccumulatedLayoutTransform = Concat(LocalLayoutTransform, ParentAccumulatedLayoutTransform)
		 * @param ParentAccumulatedRenderTransform the accumulated render transform of the parent widget, AccumulatedRenderTransform = Concat(LocalRenderTransform, LocalLayoutTransform, ParentAccumulatedRenderTransform)
		 */
		FGeometry(
			const glm::vec2& in_local_size,
			const FSlateLayoutTransform& in_local_layout_transform,
			const FSlateRenderTransform& in_local_render_transform,
			const glm::vec2& in_local_render_transform_pivot,
			const FSlateLayoutTransform& parent_accumulated_layout_transform,
			const FSlateRenderTransform& parent_accumulated_render_transform
		)
			: m_size(in_local_size)
			, m_scale(1.0f)
			, m_absolute_position(0.0f, 0.0f)
			, m_accumulated_render_transform(
				concatenate(
					//covert the pivot to local space and make it the origin
					inverse(transform_point(FScale2D(in_local_size), in_local_render_transform_pivot)),
					//apply the render transform in local space centered around the pivot
					in_local_render_transform,
					//translate the pivot back
					transform_point(FScale2D(in_local_size), in_local_render_transform_pivot),
					//apply the layout transform next
					in_local_layout_transform,
					//finally apply the parent accumulated transform, which takes us to the root
					parent_accumulated_render_transform)
			)//todo:implement concatenate
			, m_b_has_render_transform(true)
			, m_local_position(0.0f, 0.0f)
		{
			FSlateLayoutTransform accumulated_layout_transform = concatenate(in_local_layout_transform, parent_accumulated_layout_transform);

			//FSlateRenderTransform a = concatenate(//covert the pivot to local space and make it the origin
			//	inverse(transform_point(in_local_size, in_local_render_transform_pivot)),
			//	//apply the render transform in local space centered around the pivot
			//	in_local_render_transform,
			//	//translate the pivot back
			//	transform_point(in_local_size, in_local_render_transform_pivot),
			//	//apply the layout transform next
			//	in_local_layout_transform);
			//
			//concatenate(a, parent_accumulated_render_transform);
			//FSlateRenderTransform a = concatenate(concatenate(concatenate(concatenate(inverse(transform_point(in_local_size, in_local_render_transform_pivot)), in_local_render_transform), transform_point(in_local_size, in_local_render_transform_pivot)), in_local_layout_transform), parent_accumulated_render_transform);
			//FSlateRenderTransform b = concatenate(a, transform_point(in_local_size, in_local_render_transform_pivot));
			//FSlateRenderTransform c = concatenate(b, in_local_layout_transform);
			//FSlateRenderTransform d = concatenate(c, parent_accumulated_render_transform);

			const_cast<glm::vec2&>(m_absolute_position) = accumulated_layout_transform.get_translation();
			const_cast<float&>(m_scale) = accumulated_layout_transform.get_scale();
			const_cast<glm::vec2&>(m_local_position) = in_local_layout_transform.get_translation();
		}

		FGeometry(
			const glm::vec2& in_local_size,
			const FSlateLayoutTransform& in_local_layout_transform,
			//const FSlateRenderTransform& in_local_render_transform,
			//const glm::vec2& in_local_render_transform_pivot,
			const FSlateLayoutTransform& parent_accumulated_layout_transform,
			const FSlateRenderTransform& parent_accumulated_render_transform,
			bool b_parent_has_render_transform
		)
			: m_size(in_local_size)
			, m_scale(1.0f)
			, m_absolute_position(0.0f, 0.0f)//note:this is calculated from arrange children
			, m_accumulated_render_transform( concatenate(in_local_layout_transform, parent_accumulated_render_transform)
			)//todo:implement concatenate
			, m_b_has_render_transform(b_parent_has_render_transform)
			, m_local_position(0.0f, 0.0f)
		{
			FSlateLayoutTransform accumulated_layout_transform;
			//hack to allow us to make FGeometry public members immutable to catch misuse
			const_cast<glm::vec2&>(m_absolute_position) = glm::vec2(accumulated_layout_transform.get_translation());
			const_cast<float&>(m_scale) = accumulated_layout_transform.get_scale();
			const_cast<glm::vec2&>(m_local_position) = glm::vec2(in_local_layout_transform.get_translation());
		}

	public:
		/*
		 * makes a new geometry that is essentially the root of a hierarchy (has no parent transform to inherit)
		 * for a root widget, the layout transform is often the window dpi scale + window offset
		 * @param LocalSize size of the geometry in local space
		 * @param LayoutTransform layout transform of the geometry
		 * @return the new root geometry
		 */
		static FGeometry make_root(const glm::vec2& in_local_size, const FSlateLayoutTransform& layout_transform)
		{
			return FGeometry(in_local_size, layout_transform, FSlateLayoutTransform(), FSlateRenderTransform(), false);
		}

		FGeometry make_child(const glm::vec2& in_local_size, const FSlateLayoutTransform& layout_transform, const FSlateRenderTransform& render_transform, const glm::vec2& render_transform_pivot) const
		{
			return FGeometry(in_local_size, layout_transform, render_transform, render_transform_pivot, get_accumulated_layout_transform(), get_accumulated_render_transform());
		}

		FGeometry make_child(const glm::vec2& in_local_size, const FSlateLayoutTransform& layout_transform) const
		{
			return FGeometry(in_local_size, layout_transform, get_accumulated_layout_transform(), get_accumulated_render_transform(), m_b_has_render_transform);
		}

		FGeometry make_child(const FSlateRenderTransform& render_transform, const glm::vec2& render_transform_pivot = glm::vec2(0.5f, 0.5f)) const
		{
			return FGeometry(get_local_size(), FSlateLayoutTransform(), render_transform, render_transform_pivot, get_accumulated_layout_transform(), get_accumulated_render_transform());
		}

		/*
		 * create a child geometry+widget relative to this one using the given layout geometry
		 *
		 * @param ChildWidget the child widget this geometry is being created for
		 * @param LayoutGeometry layout geometry of the child
		 *
		 * @return the new child geometry
		 */
		FArrangedWidget make_child(const std::shared_ptr<SWidget>& child_widget, const FLayoutGeometry& layout_geometry) const;

		FArrangedWidget make_child(const std::shared_ptr<SWidget>& child_widget, const glm::vec2& in_local_size, const FSlateLayoutTransform& layout_transform) const;

		/*
		 * use make child taking a layout transform instead!
		 * create a child geometry+widget relative to this one with a given local space size and layout transform given by a scale+offset
		 * the geometry inherits the child widget's render transform
		 *
		 * @param child_widget the child widget this geometry is being created for
		 * @param child_offset offset of the child relative to the parent, scale+offset effectively define the layout transform
		 * @param local_size the size of the child geometry in local size
		 * @param child_scale scale of the child relative to the parent, scale+offset effectively define the layout transform
		 *
		 * @return the new geometry+widget
		 */
		FArrangedWidget make_child(const std::shared_ptr<SWidget>& child_widget, const glm::vec2& child_offset, const glm::vec2& in_local_size, float child_scale = 1.0f) const;
		/*
		 * test geometries for strict equality(i.e. extract float equality)
		 *
		 * @param Other A geometry to compare to
		 * @return true if this is equal to other, false otherwise
		 */
		bool operator==(const FGeometry& other) const
		{
			return this->m_size == other.m_size &&
				this->m_absolute_position == other.m_absolute_position &&
				this->m_local_position == other.m_local_position &&
				this->m_scale == other.m_scale;//compare float is not safe
		}

		/*
		 * test geometries for strict equality(i.e. extract float equality)
		 *
		 * @param Other A geometry to compare to
		 * @return false if this is equal to other, true otherwise
		 */
		bool operator!=(const FGeometry& other) const
		{
			return !(this->operator==(other));//check
		}

		/*
		 * create a paint geometry that represents this geometry
		 *
		 * @return the new paint geometry
		 */
		FPaintGeometry to_paint_geometry() const
		{
			return FPaintGeometry(get_accumulated_layout_transform(), get_accumulated_render_transform(), m_size, m_b_has_render_transform);
		}

		/*
		 * create a paint geometry relative to this one with a given local space size and layout transform given as a scale + offset
		 * the paint geometry inherits the widget's render transform
		 *
		 * @param LocalOffset offset of the paint geometry relative to the parent, scale + offset effectively define the layout transform
		 * @param LocalSize the size of the paint geometry in local space
		 * @param LocalScale scale of the paint geometry relative to the parent, scale + offset effectively define the layout transform
		 *
		 * @return the new paint geometry derived from this one
		 */
		FPaintGeometry to_paint_geometry(const glm::vec2& in_local_offset, const glm::vec2& in_local_size, float in_local_scale = 1.0f) const
		{
			//since child offset is given as a local space offset, we must convert this offset into the space of the parent to construct a valid layout transform
			//the extra transform point below does this by converting the local offset to an offset in parent space
			return to_paint_geometry(in_local_size, FSlateLayoutTransform(in_local_scale, transform_point(in_local_scale, in_local_offset)));
		}

		/*
		 * create a paint geometry relative to this one with a given local space size and layout transform
		 * the paint geometry inherits the widget's render transform
		 *
		 * @param local size the size of the child geometry in local space
		 * @param layout transform layout transform of the paint geometry relative to this geometry
		 *
		 * @return the new paint geometry derived from this one
		 */
		FPaintGeometry to_paint_geometry(const glm::vec2& in_local_size, const FSlateLayoutTransform& in_layout_transform) const
		{
			FSlateLayoutTransform new_accumulated_layout_transform = DoDo::concatenate(in_layout_transform, get_accumulated_layout_transform());
			return FPaintGeometry(new_accumulated_layout_transform, DoDo::concatenate(in_layout_transform, get_accumulated_render_transform()), in_local_size, m_b_has_render_transform);
		}

		FSlateRect get_layout_bounding_rect() const
		{
			return get_layout_bounding_rect(FSlateRect(glm::vec2(0.0f, 0.0f), m_size));
		}

		FSlateRect get_layout_bounding_rect(const FSlateRect& local_space_rect) const
		{
			//get_accumulated_layout_transform()
			return transform_rect(get_accumulated_layout_transform(), FSlateRotatedRect(local_space_rect)).to_bounding_rect();
		}

		FSlateRect get_render_bounding_rect()
		{
			return get_layout_bounding_rect(FSlateRect(0.0f, 0.0f, m_size.x, m_size.y) );
		}

		FSlateRect get_render_bounding_rect(const FSlateRect& local_space_rect) const
		{
			return transform_rect(get_accumulated_render_transform(), FSlateRotatedRect(local_space_rect)).to_bounding_rect();
		}

		/** @return the accumulated render transform, shouldn't be needed in general**/
		const FSlateRenderTransform& get_accumulated_render_transform() const { return m_accumulated_render_transform; }

		/*
		 * @return the accumulated layout transform, shouldn't be needed in general
		 */
		FSlateLayoutTransform get_accumulated_layout_transform() const
		{
			return FSlateLayoutTransform(m_scale, m_absolute_position);
		}

		/*
		 * special case method to append a layout transform to a geometry
		 * this is used in cases where the FGeometry was arranged in window space
		 * and we need to add the root desktop translation
		 *
		 * @param LayoutTransform an additional layout transform to append to this geometry
		 */
		void append_transform(const FSlateLayoutTransform& layout_transform)
		{
			FSlateLayoutTransform accumulated_layout_transform = DoDo::concatenate(get_accumulated_layout_transform(), layout_transform);
			m_accumulated_render_transform = DoDo::concatenate(m_accumulated_render_transform, layout_transform);
			const_cast<glm::vec2&>(m_absolute_position) = accumulated_layout_transform.get_translation();
			const_cast<float&>(m_scale) = accumulated_layout_transform.get_scale();
		}

		/* return the size of the geometry in local space*/
		const glm::vec2& get_local_size() const { return m_size; }


		/*@return the accumulated render transform, shouldn't be needed general*/
		//const FSlateRenderTransform & get_accumulated_render_transform() const { return m_accumulated_render_transform; }

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

		/*
		* absolute coordinates could be either desktop or window space depending on what space the root of the widget hierarchy is in
		* 
		* @return true if the provided location in absolute coordinates is within the bounds of this geometry
		*/
		bool is_under_location(const glm::vec2& absolute_coordinate) const
		{
			//this render transform invert is a little expensive, we might consider caching it?
			FSlateRotatedRect rect = transform_rect(get_accumulated_render_transform(), FSlateRotatedRect(FSlateRect(glm::vec2(0.0f, 0.0f), m_size)));

			return rect.is_under_location(absolute_coordinate);
		}

		/*
		* absolute coordinates could be either desktop or window space depending on what space the root of the widget hierarchy is in
		* 
		* @return transforms absolute coordinate into the local space of this geometry
		*/
		glm::vec2 absolute_to_local(glm::vec2 absolute_coordinate) const
		{
			//this render transform inverts is a little expensive, we might consider caching it
			return transform_point(inverse(get_accumulated_render_transform()), absolute_coordinate);
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
