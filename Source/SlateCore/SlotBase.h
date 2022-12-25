#pragma once

#include "SlateCore/Widgets/InvalidateWidgetReason.h"

#include "Core/Misc/Attribute.h"

namespace DoDo
{
	class FChildren;

	class SWidget;

	//class TAttribute;
	/*slot are a container of a SWidget used by the FChildren */
	class FSlotBase
	{
	public:
		FSlotBase();

		FSlotBase(const FChildren& in_parent);

		FSlotBase(const std::shared_ptr<SWidget>& in_widget);

		FSlotBase& operator=(const FSlotBase&) = delete;

		FSlotBase(const FSlotBase&) = delete;

		virtual ~FSlotBase();

	public:
		//this is empty, used by inherited
		struct FSlotArguments {};

	public:
		void attach_widget_parent(SWidget* in_parent) {}

		/*
		 * access the FChildren that own the slot
		 * the owner can be invalid when slot is not attached
		 */
		const FChildren* get_owner() const
		{
			return m_owner;
		}

		/*
		 * access the FChildren that own the slot
		 * the owner can be invalid when the slot is not attached
		 */
		SWidget* get_owner_widget() const;

		/*
		 * set the owner of the slot
		 * slots cannot be reassigned to different parents
		 */
		void set_owner(const FChildren& children);

		//attach the child widget the slow now owns
		void attach_widget(const std::shared_ptr<SWidget>& in_widget)
		{
			//todo:implement detach parent from content
			m_widget = in_widget;
			//todo:implement after content or owner assigned
		}

		/*
		 * access the widget in the current slot
		 * there will always be a widget in the slot, sometimes it is the SNullWidget instance
		 */
		const std::shared_ptr<SWidget> get_widget() const
		{
			return m_widget;
		}

		/*
		 * remove the widget from it's current slot
		 * the removed widget is returned so that operations could be performed on it
		 * if the null widget was beging stored, an invalid shared ptr is returned instead
		 */
		const std::shared_ptr<SWidget> detach_widget();

		/* invalidate the widget's owner */
		void invalidate(EInvalidateWidgetReason invalidate_reason);
	protected:
		/*
		 * performs the attribute assignment and invalidates the widget minimally based on what actually changed
		 * so if the boundness of the attribute didn't changed volatility won't need to be recalcualted
		 * returns true if the value changed
		 */
		template<typename TargetValueType, typename SourceValueType>
		bool set_attribute(TAttribute<TargetValueType>& target_value, const TAttribute<SourceValueType>& source_value, EInvalidateWidgetReason base_invalidation_reason)
		{
			return false;
		}
	private:
		void detach_parent_from_content();

		void after_content_or_owner_assigned();

	private:
		/* the children that own the slot */
		const FChildren* m_owner;

		/* the content widget of the slot */
		std::shared_ptr<SWidget> m_widget;
	};
}