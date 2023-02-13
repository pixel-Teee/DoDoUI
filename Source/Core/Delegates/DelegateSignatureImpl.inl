#pragma once

#include "Core/Templates/Identity.h"


class FDelegateBase;
class FDelegateHandle;
class IDelegateInstance;
/*
* unicast delegate template class
*/
template<typename DelegateSignature, typename UserPolicy = FDefaultDelegateUserPolicy>
class TDelegate
{

};

template<typename InRetValType, typename... ParamTypes, typename UserPolicy>
class TDelegate<InRetValType(ParamTypes...), UserPolicy> : public TDelegateBase<UserPolicy> //TDelegateBase holds the memory block
{
	using Super = TDelegateBase<UserPolicy>;//user default template parameter FDefaultDelegateUserPolicy
	using FuncType = InRetValType(ParamTypes...);
	using DelegateInstanceInterfaceType = IBaseDelegateInstance<FuncType, UserPolicy>;

	using DelegateType = typename UserPolicy::FDelegateExtras;//FDelegateBase
	using DelegateInstanceType = typename UserPolicy::FDelegateInstanceExtras;//IDelegateInstance

public:
	typedef InRetValType RetValType;
	typedef InRetValType TFuncType(ParamTypes...);

	/*helper typedefs for getting a member function pointer type for the delegate with a given payload*/
	template<typename UserClass, typename... VarTypes> using TMethodPtr		 = typename TMemFunPtrType<false, UserClass, RetValType(ParamTypes..., VarTypes...)>::Type;
	template<typename UserClass, typename... VarTypes> using TConstMethodPtr = typename TMemFunPtrType<true,  UserClass, RetValType(ParamTypes..., VarTypes...)>::Type;

	/*declare the user's shared pointer-based delegate instance types*/
	template<typename UserClass>
	struct TSPMethodDelegate : TBaseSPMethodDelegateInstance<false, UserClass, FuncType, UserPolicy>
	{
		typedef TBaseSPMethodDelegateInstance<false, UserClass, FuncType, UserPolicy> Super;//none const
		TSPMethodDelegate(const std::shared_ptr<UserClass>& in_user_object, typename Super::FMethodPtr in_method_ptr)
			: Super(in_user_object, in_method_ptr)
		{}
	};

	template<typename UserClass>
	struct TSPMethodDelegate_Const : TBaseSPMethodDelegateInstance<true, UserClass, FuncType, UserPolicy>
	{
		typedef TBaseSPMethodDelegateInstance<true, UserClass, FuncType, UserPolicy> Super;//none const
		TSPMethodDelegate_Const(const std::shared_ptr<UserClass>& in_user_object, typename Super::FMethodPtr in_method_ptr)
			: Super(in_user_object, in_method_ptr)
		{}
	};

	/*declare the user's static function delegate instance types*/
	template<typename... VarTypes> struct FStaticDelegate : TBaseStaticDelegateInstance<FuncType, UserPolicy, VarTypes...>
	{
		typedef TBaseStaticDelegateInstance<FuncType, UserPolicy, VarTypes...> Super;
		FStaticDelegate(typename Super::FFuncPtr in_func_ptr, VarTypes... vars) //note:pass static function pointer and parameter and data pay load
			: Super(in_func_ptr, vars...)
		{}
	};

public:
	/*
	 * static: creates a shared pointer-based member function delegate
	 *
	 * shared pointer delegates keep a weak reference to your object
	 * you can user execute if bound to call them
	 */
	template<typename UserClass, typename... VarTypes>
	inline static TDelegate<RetValType(ParamTypes...), UserPolicy> CreateSP(const std::shared_ptr<UserClass>& in_user_object_ref, typename TMemFunPtrType<false, UserClass, RetValType(ParamTypes..., VarTypes...)>::Type in_func, VarTypes... vars)
	{
		TDelegate<RetValType(ParamTypes...), UserPolicy> Result;
		TBaseSPMethodDelegateInstance<false, UserClass, FuncType, UserPolicy, VarTypes...>::Create(Result, in_user_object_ref, in_func, vars...);
		return Result;
	}

	template<typename UserClass, typename... VarTypes>
	inline static TDelegate<RetValType(ParamTypes...), UserPolicy> CreateSP(const std::shared_ptr<UserClass>& in_user_object_ref, typename TMemFunPtrType<true, UserClass, RetValType(ParamTypes..., VarTypes...)>::Type in_func, VarTypes... vars)
	{
		TDelegate<RetValType(ParamTypes...), UserPolicy> Result;
		TBaseSPMethodDelegateInstance<true, UserClass, FuncType, UserPolicy, VarTypes...>::Create(Result, in_user_object_ref, in_func, vars...);
		return Result;
	}

	/*
	* static: creates a raw c++ pointer global function delegate
	*/
	template<typename... VarTypes>//var types parameter is data pay load
	inline static TDelegate<RetValType(ParamTypes...), UserPolicy> CreateStatic(typename DoDo::TIdentity<RetValType(*)(ParamTypes..., VarTypes...)>::Type InFunc, VarTypes... Vars)
	{
		TDelegate<RetValType(ParamTypes...), UserPolicy> Result;
		TBaseStaticDelegateInstance<FuncType, UserPolicy, VarTypes...>::Create(Result, InFunc, Vars...);

		return Result;
	}
public:
	/*
	* default constructor
	*/
	inline TDelegate()
	{

	}

	inline ~TDelegate()
	{
		un_bind();
	}

	inline TDelegate(TDelegate&& other)
	{
		*this = std::move(other);
	}

	inline TDelegate(const TDelegate& other)
	{
		*this = other;
	}

	inline TDelegate& operator=(TDelegate&& other)
	{
		if (&other != this)
		{
			DelegateInstanceInterfaceType* other_instance = other.get_delegate_instance_protected();

			if (other_instance != nullptr)
			{
				other_instance->create_copy(*this);
			}
			else
			{
				un_bind();
			}
		}

		return *this;
	}

	inline TDelegate& operator=(const TDelegate& other)
	{
		if (&other != this)
		{
			DelegateInstanceInterfaceType* other_instance = other.get_delegate_instance_protected();

			if (other_instance != nullptr)
			{
				other_instance->create_copy(*this);
			}
			else
			{
				un_bind();
			}
		}

		return *this;
	}

public:

	RetValType execute(ParamTypes... Params) const
	{
		DelegateInstanceInterfaceType* local_delegate_instance = get_delegate_instance_protected();

		if (local_delegate_instance != nullptr)
			return local_delegate_instance->execute(Params...);//todo:implement execute if bound
	}

protected:
	DelegateInstanceInterfaceType* get_delegate_instance_protected() const
	{
		return (DelegateInstanceInterfaceType*)Super::get_delegate_instance_protected();
	}
};
