#pragma once

#include "Core/Containers/ContainerAllocationPolicies.h"

#include "Core/Templates/TypeCompatibleBytes.h"

#include "IDelegateInstance.h"


typedef TAlignedBytes<16, 16> FAlignedInlineDelegateType; //16 bytes size, and aligned for 16 bytes

typedef FHeapAllocator FDelegateAllocatorType;
/*
* base class for unicast delegates
*/
class FDelegateBase
{
	template<typename>
	friend class TMulticastDelegateBase;

	template<typename>
	friend class TDelegateBase;

protected:
	/*
	* creates and initializes a new instance
	*
	* @param InDelegateInstance the delegate instance to assign
	*/
	explicit FDelegateBase()
		: m_delegate_size(0)
	{}

	~FDelegateBase()
	{
		un_bind();
	}

	/*
	* move constructor
	*/
	FDelegateBase(FDelegateBase&& other)
	{
		m_delegate_allocator.move_to_empty(other.m_delegate_allocator);
		m_delegate_size = other.m_delegate_size;
		other.m_delegate_size = 0;
	}

	/*
	* move assignment
	*/
	FDelegateBase& operator=(FDelegateBase&& other)
	{
		un_bind();
		m_delegate_allocator.move_to_empty(other.m_delegate_allocator);
		m_delegate_size = other.m_delegate_size;
		other.m_delegate_size = 0;
		return *this;
	}
	/*
	* unbinds the delegate
	*/
	void un_bind()
	{
		if (IDelegateInstance* ptr = get_delegate_instance_protected())
		{
			ptr->~IDelegateInstance();
			m_delegate_allocator.resize_allocation(0, 0, sizeof(FAlignedInlineDelegateType));//16 bytes, 128 bits
			m_delegate_size = 0;
		}
	}

	/*
	* gets the delegate instance, not intended for use by user code
	*
	* @return the delegate instance
	* @see SetDelegateInstance
	*/
	IDelegateInstance* get_delegate_instance_protected() const
	{
		return m_delegate_size ? (IDelegateInstance*)m_delegate_allocator.get_allocation() : nullptr;
	}

	friend void* operator new(size_t Size, FDelegateBase& Base);//will call base's allocate function, to allocate Size
private:
	void* allocate(int32_t Size)
	{
		if (IDelegateInstance* current_instance = get_delegate_instance_protected())
		{
			current_instance->~IDelegateInstance();
		}

		int32_t new_delegate_size = (Size + sizeof(FAlignedInlineDelegateType) - 1) / sizeof(FAlignedInlineDelegateType);

		if (m_delegate_size != new_delegate_size)
		{
			m_delegate_allocator.resize_allocation(0, new_delegate_size, sizeof(FAlignedInlineDelegateType));
			m_delegate_size = new_delegate_size;
		}

		return m_delegate_allocator.get_allocation();
	}

private:

	//a block of memory data
	FDelegateAllocatorType::ForElementType<FAlignedInlineDelegateType> m_delegate_allocator;

	int32_t m_delegate_size;
};

//void* operator new(size_t Size, FDelegateBase& base)
//{
//	return base.allocate((int32_t)Size);
//}

inline void* operator new(size_t Size, FDelegateBase& Base)
{
	return Base.allocate((int32_t)Size);
}

struct FDefaultDelegateUserPolicy
{
	using FDelegateInstanceExtras = IDelegateInstance;
	using FDelegateExtras = FDelegateBase;
	//using FMulticastDelegateExtras;
	//todo:implement this function
};

template<typename UserPolicy>
class TDelegateBase : public UserPolicy::FDelegateExtras//note:inherited from FDelegateBase
{
	template<typename>
	friend class TMulticastDelegateBase;

	using Super = typename UserPolicy::FDelegateExtras;//FDelegateBase
public:
	/*
	* checks to see if the user object bound to this delegate is still valid
	*
	* @return true if the user object is still valid and it's safe to execute the function call
	*/
	bool is_bound() const
	{
		IDelegateInstance* ptr = Super::get_delegate_instance_protected();

		return ptr && ptr->is_safe_to_execute();
	}

	/*
	* gets a handle to the delegate
	*
	* @return the delegate instance
	*/
	FDelegateHandle get_handle() const
	{
		FDelegateHandle result;
		if (IDelegateInstance* ptr = Super::get_delegate_instance_protected())
		{
			result = ptr->get_handle();
		}

		return result;
	}
};






