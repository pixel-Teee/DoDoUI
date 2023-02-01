#pragma once

#include "Core/String/DoDoString.h"

namespace DoDo
{
	class SWidget;
	class FSlotBase;
	/*
	 * FChildren is an interface that must be implemented by all child containers
	 * it allows iteration over a list of any widget's children regardless of how
	 * the underlying widget happens to store it's children
	 *
	 * FChildren is intended to be returned by the GetChildren() method
	 */
	class FChildren
	{
	public:
		FChildren(SWidget* in_owner)
			: m_owner(in_owner)
			, m_name("Children")
		{
			//m_owner = in_owner;
		}

		FChildren(SWidget* in_owner, DoDoUtf8String in_name)
			: m_owner(in_owner)
			, m_name(in_name)
		{
			
		}

		FChildren(std::nullptr_t) = delete;

		//prevents allocation of FChildren. it creates confusion between FSlot and FChildren
		void* operator new(size_t) = delete;
		void* operator new[](size_t) = delete;

		/* return the number of children */
		virtual int32_t num() const = 0;

		/* return pointer to the widget at the specified index */
		virtual std::shared_ptr<SWidget> get_child_at(int32_t index) = 0;

		/* return const pointer to the widget at the specified index */
		virtual std::shared_ptr<const SWidget> get_child_at(int32_t index) const = 0;

		/* return the SWidget that own the FChildren */
		SWidget& get_owner() const { return *m_owner;  }

		/* applies the predicate to all the widgets contained by the FChildren */
		template<typename Predicate>
		void for_each_widget(Predicate pred)
		{
			for(int32_t index = 0; index < num(); ++index)
			{
				std::shared_ptr<SWidget> ref = get_child_at(index);

				pred(ref.get());
			}
		}

		/* return the number of slot the children has */
		virtual int32_t num_slot() const
		{
			return num();
		}

		/* return the const reference to the slot at the specified index */
		virtual const FSlotBase& get_slot_at(int32_t child_index) const = 0;

		/* return if the children can support slot with slate slot attribute */
		virtual bool Support_Slot_With_Slate_Attribute() const
		{
			return false;
		}

	protected:
		SWidget* m_owner;

	private:
		DoDoUtf8String m_name;
	};
}