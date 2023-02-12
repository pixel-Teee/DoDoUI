#pragma once

class FDelegateBase;

template<typename FuncType, typename UserPolicy, typename... VarTypes>
class TCommonDelegateInstanceState;

//note:just to implement some interfaces
template<typename InRetValType, typename... ParamTypes, typename UserPolicy, typename... VarTypes>
class TCommonDelegateInstanceState<InRetValType(ParamTypes...), UserPolicy, VarTypes...> : public IBaseDelegateInstance<InRetValType(ParamTypes...), UserPolicy>
{
public:
	using RetValType = InRetValType;

public:
	explicit TCommonDelegateInstanceState(VarTypes... Vars)
		: m_pay_load(Vars...)
		, m_handle(FDelegateHandle::EGenerateNewHandleType::GenerateNewHandle)
	{}

	//------IDelegateInstance interface------
	virtual FDelegateHandle get_handle() const final
	{
		return m_handle;
	}
	//------IDelegateInstance interface------

protected:
	//payload member variable(if any)
	std::tuple<VarTypes...> m_pay_load;

	//the handle of this delegate
	FDelegateHandle m_handle;
};

/*
* implements a delegate binding for regular c++ functions
*/
template<typename FuncType, typename UserPolicy, typename... VarTypes>
class TBaseStaticDelegateInstance;

//var types is data payload
template<typename WrappedRetValType, typename... ParamTypes, typename UserPolicy, typename... VarTypes>
class TBaseStaticDelegateInstance<WrappedRetValType(ParamTypes...), UserPolicy, VarTypes...> : public TCommonDelegateInstanceState<WrappedRetValType(ParamTypes...), UserPolicy, VarTypes...>
{
private:
	using Super = TCommonDelegateInstanceState<WrappedRetValType(ParamTypes...), UserPolicy, VarTypes...>;
	using RetValType = typename Super::RetValType;
	using UnwrappedThisType = TBaseStaticDelegateInstance<RetValType(ParamTypes...), UserPolicy, VarTypes...>;

public:
	using FFuncPtr = RetValType(*)(ParamTypes..., VarTypes...);//VarTypes is data pay load

	TBaseStaticDelegateInstance(FFuncPtr in_static_func_ptr, VarTypes... Vars)//data pay load
		: Super(Vars...)
		, m_static_func_ptr(in_static_func_ptr)
	{}

	//-------IDelegateInstance function------
	virtual bool is_safe_to_execute() const
	{
		return true;
	}
	//-------IDelegateInstance function------
public:
	//------IBaseDelegateInstance interface------
	/*
	* emplaces a copy of the delegate instance into the FDelegateBase
	*/
	virtual void create_copy(FDelegateBase& base) final //note:create a IBaseDelegateInstance to the FDelegateBase's memory block
	{
		//construct a delegate instance at the FDelegateBase's memory block
		new (base) UnwrappedThisType(*(UnwrappedThisType*)this);
	}

	/*
	* execute the delegate, if the function pointer is not valid, an error will occur
	*/
	virtual RetValType execute(ParamTypes... params) const final
	{
		//pass extra parameter after function parameter
		return std::apply(m_static_func_ptr, std::make_tuple(params...));//todo:need to handle extra parameter for pay load
	}
	//------IBaseDelegateInstance interface------
public:
	/*
	* creates a new static function delegate binding for the given function pointer
	*
	* @param InFunc static function pointer
	* @return the new delegate
	*/
	inline static void Create(FDelegateBase& base, FFuncPtr in_func, VarTypes... vars)
	{
		new (base) UnwrappedThisType(in_func, vars...);
	}
private:
	//c++ function pointer
	FFuncPtr m_static_func_ptr;
};
