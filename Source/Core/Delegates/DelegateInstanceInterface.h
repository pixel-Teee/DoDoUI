#pragma once

#include <tuple>

class FDelegateBase;

template<typename FuncType, typename UserPolicy>
struct IBaseDelegateInstance;

//construct a delegate instance at the FDelegateBase
template<typename RetType, typename... ArgTypes, typename UserPolicy>
struct IBaseDelegateInstance<RetType(ArgTypes...), UserPolicy> : public UserPolicy::FDelegateInstanceExtras //note:inherited from IDelegateInstance
{
	/*
	* emplaces a copy of the delegate instance into the FDelegateBase
	*/
	virtual void create_copy(FDelegateBase& base) = 0;//note:create a IBaseDelegateInstance to the FDelegateBase's memory block

	/*
	* execute the delegate, if the function pointer is not valid, an error will occur
	*/
	virtual RetType execute(ArgTypes...) const = 0;

	virtual bool execute_if_safe(ArgTypes...) const = 0;
};

template<bool Const, typename Class, typename FuncType>
struct TMemFunPtrType;

template<typename Class, typename RetType, typename... ArgTypes>
struct TMemFunPtrType<false, Class, RetType(ArgTypes...)>
{
	typedef RetType(Class::* Type)(ArgTypes...);
};

template<typename Class, typename RetType, typename... ArgTypes>
struct TMemFunPtrType<true, Class, RetType(ArgTypes...)>
{
	typedef RetType(Class::* Type)(ArgTypes...) const;
};

template<typename FuncType>
struct TPayload;

template<typename Ret, typename... Types>
struct TPayload<Ret(Types...)>
{
	std::tuple<Types..., Ret> Values;

	template<typename... ArgTypes>
	explicit TPayload(ArgTypes&&... Args)
		: Value(std::forward<ArgTypes>(Args)..., Ret())
	{

	}

	Ret& get_result()
	{
		return std::get<sizeof...(Types)>(Values);
	}
};

template<typename... Types>
struct TPayload<void(Types...)>
{
	std::tuple<Types...> Values;

	template<typename... ArgTypes>
	explicit TPayload(ArgTypes&&... Args)
		: Value(std::forward<ArgTypes>(Args)...)
	{

	}

	void get_result()
	{

	}
};
