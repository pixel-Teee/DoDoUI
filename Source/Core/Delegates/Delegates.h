#pragma once

#include <vector>
#include <cassert>

#include "IDelegateInstance.h"

namespace DoDo {
	template<typename TSignature> class Delegate;
	template<typename TSignature> class Delegate_Event;
	template<typename Return_Type, typename... Delegate_Template_Type>
	class Delegate<Return_Type(Delegate_Template_Type...)>
	{
	public:
		typedef Return_Type(*Static_Function_Type)(void* p, Delegate_Template_Type...);
		
		template<typename T, typename... Var_Types>
		using TConstMethodPtr = Return_Type(T::*)(Var_Types...) const;//const method type

		Delegate()
		{
			m_p = nullptr;
			m_f = nullptr;
			//todo:not generate new handle?
		}

		~Delegate() {}

		Delegate(const Delegate& delegate)
		{
			*this = delegate;
		}

		void operator=(const Delegate& delegate)
		{
			m_f = delegate.m_f;
			if (!delegate.m_lambda_buffer.size())
			{
				m_p = delegate.m_p;
				m_lambda_buffer.clear();
			}
			else
			{
				//directly copy the buffer
				m_lambda_buffer = delegate.m_lambda_buffer;
				m_p = (void*)m_lambda_buffer.data();
			}
		}
	protected:

		Delegate(void* p, Static_Function_Type function, uint32_t buffer_size)
		:m_delegate_handle(FDelegateHandle::EGenerateNewHandleType::GenerateNewHandle)
		{
			m_f = function;
			//if provide buffer_size, then copy the object instance directly
			if (buffer_size)
			{
				m_lambda_buffer.resize(buffer_size);
				memcpy(m_lambda_buffer.data(), p, buffer_size);
				m_p = (void*)m_lambda_buffer.data();
			}
			else
			{
				m_p = p;
			}
		}

		//object instance pointer
		void* m_p;
		//function
		Static_Function_Type m_f;
		std::vector<unsigned char> m_lambda_buffer;

		//delegate handle
		FDelegateHandle m_delegate_handle;

	public:

		template<class T, Return_Type(T::*Fun_Name)(Delegate_Template_Type...)>
		static Return_Type Method_Stub(void* p, Delegate_Template_Type... delegate_value)
		{
			T* ap = (T*)p;
			return (ap->*Fun_Name)(delegate_value...);
		}

		template<class T, Return_Type(T::*Fun_Name)(Delegate_Template_Type...)const>
		static Return_Type Const_Method_Stub(void* p, Delegate_Template_Type... delegate_value)
		{
			T* ap = (T*)p;
			return (ap->*Fun_Name)(delegate_value...);
		}

		//template<class T>
		//static Return_Type Const_Method_Stub(void* p, TConstMethodPtr<T, Delegate_Template_Type...> method, Delegate_Template_Type... delegate_value)
		//{
		//	T* ap = (T*)p;
		//	return (ap->*method)(delegate_value...);
		//}

		template<Return_Type(*Fun_Name)(Delegate_Template_Type...)>
		static Return_Type Fun_Stub(void* p, Delegate_Template_Type... delegate_value)
		{
			return (Fun_Name)(delegate_value...);
		}

		template<typename Lambda>
		static Return_Type Lambda_Stub(void* p, Delegate_Template_Type... delegate_value)
		{
			return (*((Lambda*)p))(delegate_value...);
		}

	public:

		static Delegate Create(void* p, Static_Function_Type function)
		{
			return Delegate(p, function, 0);
		}

		template<typename Lambda>
		static Delegate Create(Lambda& L, Static_Function_Type function)
		{
			return Delegate((void*)&L, function, sizeof(Lambda));
		}

		template<class T, Return_Type(T::*Fun_Name)(Delegate_Template_Type...)const>
		static Delegate From_Method(T* p)
		{
			return Create((void*)p, &Const_Method_Stub<T, Fun_Name>);
		}

		template<class T, Return_Type(*Fun_Name)(Delegate_Template_Type...)>
		static Delegate From_Method(T* p)
		{
			return Create((void*)p, &Method_Stub<T, Fun_Name>);
		}

		//template<class T, typename... Var_Types>
		//static Delegate From_Method(T* p, TConstMethodPtr method_ptr)
		//{
		//	return Create((void*)p, &Const_Method_Stub<T>);//pass pointer
		//}

		template<Return_Type(*Fun_Name)(Delegate_Template_Type...)>
		static Delegate From_Fun()
		{
			return Create(nullptr, &Fun_Stub<Fun_Name>);
		}
		
		template<typename Lambda>
		static Delegate From_Lambda(Lambda&& l)
		{
			return Create<Lambda>(l, &Lambda_Stub<Lambda>);
		}

		Return_Type Execute(Delegate_Template_Type... delegate_value) const
		{
			return (*m_f)(m_p, delegate_value...);
		}
		
		Return_Type operator()(Delegate_Template_Type... delegate_value) const
		{
			return (*m_f)(m_p, delegate_value...);
		}

		operator bool() const
		{
			return m_f != nullptr;
		}

		bool operator!() const
		{
			return !(operator bool());
		}

		bool operator==(const Delegate& rhs) const
		{
			return (m_p == rhs.m_p && m_f == rhs.m_f);
		}

		bool operator!=(const Delegate& rhs) const
		{
			return (m_p != rhs.m_p || m_f != rhs.m_f);
		}

		bool Is_Bound() const
		{
			return m_f != nullptr;
		}

		FDelegateHandle Get_Handle() const
		{
			return m_delegate_handle;
		}
	};

	template<typename Return_Type, typename... Delegate_Template_Type>
	class Delegate_Event<Return_Type(Delegate_Template_Type...)>
	{
	public:
		typedef Delegate<Return_Type(Delegate_Template_Type...)> Handler;

	public:
		Delegate_Event() {}

		~Delegate_Event() {}

		void operator+=(const Handler& handler)
		{
			this->Add(handler);
		}

		void Add(const Handler& handler)
		{
			assert(!(this->Has(handler)));

			m_handlers.push_back(handler);
		}

		template<Return_Type(*Fun_Name)(Delegate_Template_Type...)>
		void Add_Fun()
		{
			//create a function, put it to the vector
			Add(Handler::template FromFun<Fun_Name>());
		}

		template<class T, Return_Type(T::*Fun_Name)(Delegate_Template_Type...)const>
		void Add_Method(T* p)
		{
			Add(Handler::template From_Method<T, Fun_Name>(p));
		}

		template<class T, Return_Type(T::*Fun_Name)(Delegate_Template_Type...)>
		void Add_Method(T* p)
		{
			Add(Handler::template From_Method<T, Fun_Name>(p));
		}

		void operator-=(const Handler& handler)
		{
			this->Remove(handler);
		}

		void Remove(const Handler& handler)
		{
			auto iter = std::find(m_handlers.begin(), m_handlers.end(), handler);

			if (iter != m_handlers.end())
			{
				m_handlers.erase(iter);
			}
		}
		
		template<Return_Type(*Fun_Name)(Delegate_Template_Type...)>
		void Remove_Fun()
		{
			Remove(Handler::template From_Fun<Fun_Name>());
		}

		template<class T, Return_Type(T::*Fun_Name)(Delegate_Template_Type...)const>
		void Remove_Method(T* p)
		{
			Remove(Handler::template From_Method<T, Fun_Name>(p));
		}

		template<class T, Return_Type(T::* Fun_Name)(Delegate_Template_Type...)>
		void Remove_Method(T* p)
		{
			Remove(Handler::template From_Method<T, Fun_Name>(p));
		}

		bool Has(const Handler& handler)
		{
			auto iter = std::find(m_handlers.begin(), m_handlers.end(), handler);

			if (iter != m_handlers.end())
			{
				return true;
			}
			return false;
		}

		bool Is_Valid() const
		{
			return m_handlers.size() > 0;
		}
	
		void Reset()
		{
			m_handlers.clear();
		}
	
		void operator()(Delegate_Template_Type... delegate_value) const
		{
			this->Invoke(delegate_value...);
		}

		void Invoke(Delegate_Template_Type... delegate_value) const
		{
			for (size_t i = 0; i < m_handlers.size(); ++i)
			{
				m_handlers[i](delegate_value...);
			}
		}

		void Invoke_With_Emitter(Delegate_Template_Type... delegate_value, const Handler& emitter) const
		{
			for (size_t i = 0; i < m_handlers.size(); ++i)
			{
				if (m_handlers[i] != emitter)
				{
					m_handlers[i](delegate_value...);
				}
			}
		}

	private:
		typedef std::vector<Handler> delegate_list;
		delegate_list m_handlers;
	};

	//simple delegate, non return, non parameter
	typedef DoDo::Delegate_Event<void()> FSimpleDelegate;
}
