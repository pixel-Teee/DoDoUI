#pragma once

namespace DoDo
{
	template<class ContainerType>
	class TLinkedListIteratorBase
	{
	public:
		explicit TLinkedListIteratorBase(ContainerType* first_link)
			: m_current_link(first_link)
		{}

		/*
		 * advances the iterator to the next element
		 */
		void next()
		{
			m_current_link = (ContainerType*)m_current_link->get_next_link();
		}

		TLinkedListIteratorBase& operator++()
		{
			next();
			return *this;
		}

		TLinkedListIteratorBase operator++(int)
		{
			auto tmp = *this;
			next();
			return tmp;
		}

		explicit operator bool() const
		{
			return m_current_link != nullptr;
		}

	protected:
		ContainerType* m_current_link;

		friend bool operator==(const TLinkedListIteratorBase& lhs, const TLinkedListIteratorBase& rhs) { return lhs.m_current_link == rhs.m_current_link; }
		friend bool operator!=(const TLinkedListIteratorBase& lhs, const TLinkedListIteratorBase& rhs) { return lhs.m_current_link != rhs.m_current_link; }
	};

	template<class ContainerType, class ElementType>
	class TIntrusiveLinkedListIterator : public TLinkedListIteratorBase<ContainerType>
	{
		typedef TLinkedListIteratorBase<ContainerType> Super;

	public:
		explicit TIntrusiveLinkedListIterator(ContainerType* first_link)
			: Super(first_link)
		{}

		ElementType& operator->() const
		{
			return *(this->m_current_link);
		}

		ElementType& operator*() const
		{
			return *(this->m_current_link);
		}
	};

	/*
	 * base linked list class, used to implement methods shared by intrusive/non-intrusive linked list
	 */
	template<class ContainerType, class ElementType, template<class, class> class IteratorType>
	class TLinkedListBase
	{
	public:
		/*
		 * used to iterate over the elements of a linked list
		 */
		typedef IteratorType<ContainerType, ElementType> TIterator;
		typedef IteratorType<ContainerType, const ElementType> TConstIterator;

		/*
		 * default constructor (empty list)
		 */
		TLinkedListBase()
			: next_link(nullptr)
			, prev_link(nullptr)
		{}

		/*
		 * removes this element from the list in constant time
		 *
		 * this function is safe to call even if the element is not linked
		 */
		void un_link()
		{
			if(next_link)
			{
				next_link->prev_link = prev_link;
			}
			if(prev_link)
			{
				*prev_link = next_link;
			}

			//make it safe to call unlink again
			next_link = nullptr;
			prev_link = nullptr;
		}

		/*
		 * adds the element to a list, before the given element
		 *
		 * @param before the link to insert this element before
		 */
		void link_before(ContainerType* before)
		{
			prev_link = before->prev_link;
			before->prev_link = &next_link;

			next_link = before;

			if(prev_link != nullptr)
			{
				*prev_link = (ContainerType*)this;
			}
		}

		/*
		 * adds this element to the linked list, after the specified element
		 *
		 * @param After the link to insert this element after
		 */
		void link_after(ContainerType* after)
		{
			prev_link = &after->next_link;
			next_link = *prev_link;
			*prev_link = (ContainerType*)this;

			if(next_link != nullptr)
			{
				next_link->prev_link = &next_link;
			}
		}

		/*
		 * adds this element to the linked list, replacing the specified element
		 * this is equivalent to calling link_before(replace), replace->un_link()
		 *
		 * @param Replace pointer to the element to be replaced
		 */
		void link_replace(ContainerType* replace)
		{
			ContainerType**& replace_prev = replace->prev_link;
			ContainerType*& replace_next = replace->next_link;

			prev_link = replace_prev;
			next_link = replace_next;

			if(prev_link != nullptr)
			{
				*prev_link = (ContainerType*)this;
			}

			if(next_link != nullptr)
			{
				next_link->prev_link = &next_link;
			}

			replace_prev = nullptr;
			replace_next = nullptr;
		}

		/*
		 * adds this element as the head of the linked list, linking the input head pointer to this element
		 * so that when the element is linked/unlinked, the head linked list pointer will be correctly updated
		 *
		 * if head already has an element, this functions like linkbefore
		 *
		 * @param head pointer to the head of the linked list - this pointer should be the main reference pointer for the linked list
		 */
		void link_head(ContainerType*& head)
		{
			if(head != nullptr)
			{
				head->prev_link = &next_link;
			}

			next_link = head;
			prev_link = &head;
			head = (ContainerType*)this;
		}

		/*
		 * returns whether element is currently linked
		 *
		 * @return true if currently linked, false otherwise
		 */
		bool is_linked()
		{
			return prev_link != nullptr;
		}

		ContainerType* get_prev_link() const
		{
			return prev_link;
		}

		ContainerType* get_next_link() const
		{
			return next_link;
		}

		ContainerType* next()
		{
			return next_link;
		}

	private:
		/*the next link in the linked list*/
		ContainerType* next_link;

		/*pointer to 'next_link', within the previous link in the linked list*/
		ContainerType** prev_link;

		friend TIterator	  begin(	  ContainerType& list)	   { return TIterator(&list); }
		friend TConstIterator begin(const ContainerType& list)     { return TConstIterator(const_cast<ContainerType*>(&list)); }
		friend TIterator      end  (	  ContainerType& list)     { return TIterator(nullptr); }
		friend TConstIterator end  (const ContainerType& list)     { return TConstIterator(nullptr); }
	};

	template<class ElementType>
	class TIntrusiveLinkedList : public TLinkedListBase<ElementType, ElementType, TIntrusiveLinkedListIterator>
	{
		typedef TLinkedListBase<ElementType, ElementType, TIntrusiveLinkedListIterator> Super;

	public:
		/*default constructor (empty list). */
		TIntrusiveLinkedList()
			: Super()
		{
			
		}
	};
}