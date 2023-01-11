#pragma once

#include <Core/Core.h>

#include <type_traits>

#include "SlateCore/Types/SlateAttribute.h"
#include "SlateCore/Widgets/SlateControlledConstruction.h"

#include "SlateCore/Layout/Visibility.h"//m_Visibility_Attribute depends on it(EVisibility)

#include "glm/glm.hpp"

#include "SlateCore/Rendering/SlateRenderTransform.h"//m_render_transform_attribute depends on it

#include <optional>//std::optional depends on it

namespace DoDo
{
	struct FSlateBaseNamedArgs;
	class FChildren;

	class ISlateMetaData;

	class FPaintArgs;
	struct FGeometry;
	class FSlateRect;
	class FSlateWindowElementList;
	class FWidgetStyle;
	class FArrangedChildren;

	class SWidget : public FSlateControlledConstruction
	{
	public:
		//widgets should only ever be constructed via SNew or SAssignNew
		//todo:need to implement Private_Register Attributes static function
		SLATE_DECLARE_WIDGET(SWidget, FSlateControlledConstruction)

		friend class FSlateAttributeMetaData;

		template<class WidgetType, typename RequiredArgsPayloadType>
		friend struct TSlateDecl;//for SWidget construct use

		//SWidget();

	public:
		virtual ~SWidget();

	public:
		//layout

		/* return true if the widgets will update its registered slate attributes automatically or they need to be updated manually */
		bool Is_Attributes_Updates_Enabled() const { return m_b_enabled_attributes_update; }

		/* return the desired size that was computed the last time CachedDesiredSize() was called */
		glm::vec2 get_desired_size() const;

		void assign_parent_widget(std::shared_ptr<SWidget> in_parent);
		/* be used by FSlotBase to detach this widget from parent widget*/
		bool conditionally_detach_parent_widget(SWidget* in_expected_parent);

	protected:
		/*
		 * compute the ideal size necessary to display this widget. for aggregate widgets (e.g. panels) this size should include the
		 * size necessary to show all of it's children. CacheDesiredSize() guarantess that the size of descendatnts is computed and cached
		 * before that of the parents, so it is safe to call GetDesiredSize() for any children while implementing this method
		 *
		 * note that ComputeDesiredSize() is meant as an aide to the developer. it is not meant to be very roubust in many case
		 *
		 * @param LayoutScaleMultiplier this parameter is safe to ignore for almost all widgets, only really affects text measuring
		 *
		 * @return the desired size
		 */
		virtual glm::vec2 Compute_Desired_Size(float Layout_Scale_Multiplier) const = 0;

		/*
		 * non-virtual entry point for arrange children
		 * arrange children function
		 * compute the geometry of all the children and add populate the arranged children list with their values
		 * each type of layout panel should arrange children based on desired behaviour
		 *
		 * optionally, update the collapsed attributes(attributes that affect the visibility) of the children before executing the virtual arrange children function
		 * the visibility attribute is updated once per frame(see slate pre pass)
		 * use the option when you are calling arrange children outside of the regular SWidget Paint/Tick
		 *
		 * @param AllottedGeometry the geometry allotted for this widget by it's parent
		 * @param ArrangedChildren the array to which to add the widget geometries that represent the arranged children
		 * @param bUpdateAttributes update the collapsed attributes
		 */
		void Arrange_Children(const FGeometry& allotted_geometry, FArrangedChildren& arranged_children, bool b_update_attributes = false) const;

		/*
		 * returns the useful children(if any) of this widget. some widget type may hide widget if they are needed by the system.
		 * allows for iteration over the widget's children regardless of how they are actually stored
		 * @note should be renamed to GetVisibleChidlren(not all children will be returned in all cases)
		 */
		virtual FChildren* Get_Children() = 0;

		/*
		 * compute the geometry of all the children and add populate the arranged children list with their values
		 * each type of layout panel should arrange children based on desired behaviour
		 *
		 * @param AllottedGeometry the geometry allotted for this widget by it's parent
		 * @param ArrangedChildren the array to which to add the widget geometries that represent the arranged children
		 */
		virtual void On_Arrange_Children(const FGeometry& allotted_geometry, FArrangedChildren& arranged_children) const = 0;

	public:
		/*
		 * @return is this widget visible, hidden or collapsed
		 * @note this widget can be visible but if a parent is hidden or collapsed, it would not show on screen
		 */
		EVisibility get_visibility() const { return m_Visibility_Attribute.Get(); }
		/*
		 * invalidates the widget from the view of a layout caching widget that may own this widget
		 * will force the owning widget to redraw and cache children on the next paint pass
		 */
		void Invalidate(EInvalidateWidgetReason in_validate_reason);

	protected:
		/*
		a slate attribute that is member variable of a SWidget
		usage : TSlateAttribute<int32_t> MyAttribute1; TSlateAttribute<int32_t, EInvalidateWidgetReason::Paint> MyAttribute2; TSlateAttribute<int32_t, EInvalidateWidgetReason::Paint, TSlateAttributeComparePredicate<>> MyAttribute3;
		*/
		template<typename InObjectType, EInvalidateWidgetReason InInvalidationReasonValue = EInvalidateWidgetReason::None, typename InComparePredicate = TSlateAttributeComparePredicate<>>
		struct TSlateAttribute : public SlateAttributePrivate::TSlateMemberAttribute<
			InObjectType,
			typename std::conditional<InInvalidationReasonValue == EInvalidateWidgetReason::None,
			SlateAttributePrivate::FSlateAttributeNoInvalidationReason,
			TSlateAttributeInvalidationReason<InInvalidationReasonValue>>::type,
			InComparePredicate>
		{
			using SlateAttributePrivate::TSlateMemberAttribute<
				InObjectType,
				typename std::conditional<InInvalidationReasonValue == EInvalidateWidgetReason::None,
				SlateAttributePrivate::FSlateAttributeNoInvalidationReason,
				TSlateAttributeInvalidationReason<InInvalidationReasonValue>>::type,
				InComparePredicate>::TSlateMemberAttribute;
		};

	protected:
		/*
		 * hidden default constructor
		 *
		 * use SNew(WidgetClassName) to instantiate new widgets
		 *
		 * @see SNew
		 */
		SWidget();

		/*construct a SWidget based on initial parameters*/
		void SWidgetConstruct(const FSlateBaseNamedArgs& args);

		/* is the widget construction completed(did we called and returned from the Construct() function) */
		bool Is_Constructed() const { return m_b_Is_Declarative_Syntax_Construction_Completed; }

	private:
		/*
		 * the widget should respond by populating the OutDrawElements array with FDrawElements
		 * that represent it and any of it's children, called by the non-virtual OnPaint to enforce pre/post conditions
		 * during OnPaint
		 *
		 * @param Args all the arguments necessary to paint this widget(@todo umg:move all params into this struct)
		 * @param AllottedGeometry the FGeometry that describes an area in which the widget should appear
		 * @param MyCullingRect the rectangle representing the bounds currently being to completely cull widgets
		 * @param OutDrawElements a list of FDrawElements to populate with the output
		 * @param LayerId the layer onto which this widget should be rendered
		 * @param InColorAndOpacity color and opacity to be applied to all the descendants of the widget begin painted
		 * @param bParentEnabled true if the parent of this enabled
		 * @return the maximum layer id attained by this widget or any of it's children
		 */
		virtual int32_t On_Paint(const FPaintArgs& args, const FGeometry& allotted_geometry, const FSlateRect& my_culling_rect, FSlateWindowElementList& out_draw_elements,
			int32_t layer_id, const FWidgetStyle& in_widget_style, bool b_parent_enabled) const = 0;
	private:
		/* are bound slate attributes will be updated once per frame */
		uint8_t m_b_enabled_attributes_update : 1;

		/* the SNew or SAssignedNew construction is completed */
		uint8_t m_b_Is_Declarative_Syntax_Construction_Completed : 1;

	protected:
		/*
		 * called when a child is removed from the tree parent's widget tree either by removing it from a slot
		 * this can also be called manually if you've got some non-slot based what of no longer reporting children
		 * an example of a widget that needs manual calling is SWidgetSwitcher
		 * it keeps all its children but only arranges and paints a single "active" one
		 * once a child becomes inactive, once a child becomes inactive, it's cached data should be removed
		 */
		void invalidate_child_remove_from_tree(SWidget& child);

	public:
		//return true if the widgets has any bound slate attribute
		bool Has_Registered_Slate_Attribute() const { return m_b_has_registered_slate_attribute; }

		/* pointer to this widgets parent widget. if it is null this is a root widget or it is not in the widget tree */
		std::weak_ptr<SWidget> m_parent_widget_ptr;

		//todo:move this member to private control
		//meta data associated with this widget
		std::vector<std::shared_ptr<ISlateMetaData>> m_Meta_Data;
	private:
		/* stores the ideal size this widget wants to be */
		std::optional<glm::vec2> m_desired_size;

		/* is this widget visible, hidden or collapsed */
		TSlateAttribute<EVisibility> m_Visibility_Attribute;

		/* whether or not this widget is enabled */
		TSlateAttribute<bool> m_enabled_state_attribute;

		/* render transform pivot of this widget(in normalized local space) */
		TSlateAttribute<glm::vec2> m_render_transform_pivot_attribute;

		/* render transform of this widget. TOptional<> to allow code to skip expensive overhead if there is no render transform applied */
		//TSlateAttribute<std::optional<FSlateRenderTransform>>
		//todo:add std::optional<>
		TSlateAttribute<FSlateRenderTransform> m_render_transform_attribute;

		//is there at least one slate attribute currently registered
		uint8_t m_b_has_registered_slate_attribute : 1;
	};
}