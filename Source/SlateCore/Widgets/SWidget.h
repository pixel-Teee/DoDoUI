#pragma once

#include <Core/Core.h>

#include <type_traits>

#include "SlateCore/Types/SlateAttribute.h"
#include "SlateCore/Widgets/SlateControlledConstruction.h"

#include "SlateCore/Layout/Visibility.h"//m_Visibility_Attribute depends on it(EVisibility)

#include "glm/glm.hpp"

#include "SlateCore/Rendering/SlateRenderTransform.h"//m_render_transform_attribute depends on it

namespace DoDo
{

	class ISlateMetaData;

	class SWidget : public FSlateControlledConstruction
	{
	public:
		//widgets should only ever be constructed via SNew or SAssignNew
		//todo:need to implement Private_Register Attributes static function
		SLATE_DECLARE_WIDGET(SWidget, FSlateControlledConstruction)

		friend class FSlateAttributeMetaData;

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

		/* is the widget construction completed(did we called and returned from the Construct() function) */
		bool Is_Constructed() const { return m_b_Is_Declarative_Syntax_Construction_Completed; }

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