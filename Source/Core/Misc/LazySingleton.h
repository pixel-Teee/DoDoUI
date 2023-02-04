#pragma once

#include <new>

namespace DoDo {
	class FLazySingleton
	{
	protected:
		template<class T> static void Construct(void* Place) { new (Place) T; }

		template<class T> static void Destruct(T* Instance) { Instance->~T(); }
	};

	template<class T>
	class TLazySingleton final : public FLazySingleton
	{
	public:
		static T& Get()
		{
			return GetLazy(Construct<T>).GetValue();
		}

		static void TearDown()//singleton will automatic destruct, we can call this to explicit destruct
		{
			return GetLazy(nullptr).Reset();
		}

		static T* TryGet()
		{
			return GetLazy(Construct<T>).TryGetValue();
		}

	private:
		static TLazySingleton& GetLazy(void(*Constructor)(void*))//constructor call back
		{
			static TLazySingleton singleton(Constructor);

			return singleton;
		}

		alignas(T) unsigned char Data[sizeof(T)];//this is data
		T* Ptr;

		TLazySingleton(void(*Constructor)(void*))
		{
			if (Constructor)
			{
				Constructor(Data);
			}

			Ptr = Constructor ? (T*)Data : nullptr;
		}

		~TLazySingleton()
		{
			Reset();//todo:add marco for close auto destructor
		}

		T* TryGetValue()
		{
			return Ptr;
		}

		T& GetValue()
		{
			return *Ptr;
		}

		void Reset()
		{
			if (Ptr)
			{
				Destruct(Ptr);
				Ptr = nullptr;
			}
		}
	};
}