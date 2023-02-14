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
 * implements a delegate binding for shared pointer member functions
 */
template<bool bConst, class UserClass, typename FuncType, typename UserPolicy, typename... VarTypes>
class TBaseSPMethodDelegateInstance;

//bConst control this member function is const
template<bool bConst, class UserClass, typename WrappedRetValType, typename... ParamTypes, typename UserPolicy, typename... VarTypes>
class TBaseSPMethodDelegateInstance<bConst, UserClass, WrappedRetValType(ParamTypes...), UserPolicy, VarTypes...> : public TCommonDelegateInstanceState<WrappedRetValType(ParamTypes...), UserPolicy, VarTypes...>
{
private:
	using Super				= TCommonDelegateInstanceState<WrappedRetValType(ParamTypes...), UserPolicy, VarTypes...>;
	using RetValType		= typename Super::RetValType;
	using UnwrappedThisType = TBaseSPMethodDelegateInstance<bConst, UserClass, RetValType(ParamTypes...), UserPolicy, VarTypes...>;

public:
	using FMethodPtr		= typename TMemFunPtrType<bConst, UserClass, RetValType(ParamTypes..., VarTypes...)>::Type;

	TBaseSPMethodDelegateInstance(const std::shared_ptr<UserClass>& in_user_object, FMethodPtr in_method_ptr, VarTypes... vars)
		: Super(vars...)
		, m_user_object(in_user_object)
		, m_method_ptr(in_method_ptr)
	{}

public:
	//-------IDelegateInstance function------
	virtual bool is_safe_to_execute() const
	{
		return true;
	}
	//-------IDelegateInstance function------

	//------IBaseDelegateInstance interface------
	void create_copy(FDelegateBase& base) final
	{
		new (base) UnwrappedThisType(*(UnwrappedThisType*)this);
	}

	RetValType execute(ParamTypes... params) const final
	{
		using MutableUserClass = typename std::remove_const<UserClass>::type;

		std::shared_ptr<UserClass> shared_user_object = m_user_object.lock();

		if(shared_user_object)
		{
			MutableUserClass* mutable_user_object = const_cast<MutableUserClass*>(shared_user_object.get());

			return std::apply(m_method_ptr, std::make_tuple(mutable_user_object, params...));
		}

		return RetValType();//todo:fix me
	}

	bool execute_if_safe(ParamTypes... params) const final
	{
		if(std::shared_ptr<UserClass> shared_user_object = this->m_user_object.lock())
		{
			using MutableUserClass = typename std::remove_const<UserClass>::type;

			MutableUserClass* mutable_user_object = const_cast<MutableUserClass*>(shared_user_object.get());

			std::apply(m_method_ptr, std::make_tuple(mutable_user_object, params...));

			return true;
		}
		return false;
	}
	//------IBaseDelegateInstance interface------
public:
	/*
	 * creates a new shared pointer delegate binding for the given user object and method pointer
	 *
	 * @param InUserObjectRef shared reference to the user's object that contains the class method
	 * @param InFunc Member function pointer to tour class method
	 * @return the new delegate
	 */
	static void Create(FDelegateBase& base, const std::shared_ptr<UserClass>& in_user_object_ref, FMethodPtr in_func, VarTypes... vars)
	{
		new (base) UnwrappedThisType(in_user_object_ref, in_func, vars...);
	}
protected:
	//weak reference to an instance of the user's class which contains a method we would like to call
	std::weak_ptr<UserClass> m_user_object;

	//c++ member function pointer
	FMethodPtr m_method_ptr;
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

	bool execute_if_safe(ParamTypes... params) const final
	{
		std::apply(m_static_func_ptr, std::make_tuple(params...));

		return true;
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
