#pragma once

#include "Core/Core.h"


template<int32_t IndexSize>
struct TBitsToSizeType
{
	//todo:implement assert
};

template<> struct TBitsToSizeType<8> { using Type = int8_t; };
template<> struct TBitsToSizeType<16> { using Type = int16_t; };
template<> struct TBitsToSizeType<32> { using Type = int32_t; };
template<> struct TBitsToSizeType<64> { using Type = int64_t; };

/*the indirect allocation policy always allocates the elements indirectly*/
template<int32_t IndexSize>
class TSizedHeapAllocator
{
public:
	using SizeType = typename TBitsToSizeType<IndexSize>::Type;

	class ForAnyElementType//note:for any element type just for any element type, but ForElementType will just need you to support specific type
	{
		template<int32_t>
		friend class TSizedHeapAllocator;

	public:
		/*default constructor*/
		ForAnyElementType()
			: m_data(nullptr)
		{}

		/*
		* moves the state of another allocator into this one, the allocator can be different
		*
		* assumes that the allocator is currently empty, i.e. memory may be allocated but any existing elements have already been destructed(if necessary)
		* @param other - the allocator to move the state from, this allocator should be left in a valid empty state
		*/
		template<typename OtherAllocator>
		void move_to_empty_from_other_allocator(typename OtherAllocator::ForAnyElementType& other)
		{
			//todo:check different

			if (m_data)
			{
				std::free(m_data);
			}

			m_data = other.m_data;
			other.m_data = nullptr;
		}

		void* get_allocation() const
		{
			return m_data;
		}

		//realloc will save the original content
		void resize_allocation(SizeType previous_num_elements, SizeType num_elements, size_t num_bytes_per_element) //note:reallocate to num_elements * num_bytes_per_element
		{
			if (m_data || num_elements)
			{
				m_data = std::realloc(m_data, num_elements * num_bytes_per_element);
			}
		}

		size_t get_allocated_size(SizeType num_allocated_elements, size_t num_bytes_per_element) const //note:just to calculate the parameter
		{
			return num_allocated_elements * num_bytes_per_element;
		}

		bool has_allocation() const
		{
			return !!m_data;
		}

		/*
		* moves the state of another allocator into this one
		* moves the state of another allocator into this one, the allocator can be different
		*
		* assumes that the allocator is currently empty, i.e. memory may be allocated but any existing elements have already been destructed(if necessary)
		* @param other - the allocator to move the state from, this allocator should be left in a valid empty state
		*/
		void move_to_empty(ForAnyElementType& other)
		{
			this->move_to_empty_from_other_allocator<TSizedHeapAllocator>(other);
		}
	private:
		ForAnyElementType(const ForAnyElementType&);//forbidden
		ForAnyElementType& operator=(const ForAnyElementType&);//forbidden

		/*a pointer to the container's elements*/
		void* m_data;
	};

	template<typename ElementType> //note:get allocation return type to cast specific type
	class ForElementType : public ForAnyElementType
	{
	public:
		ForElementType()
		{

		}

		ElementType* get_allocation() const
		{
			return (ElementType*)ForAnyElementType::get_allocation();
		}
	};
};

using FHeapAllocator = TSizedHeapAllocator<32>;//32 just function calculate type
