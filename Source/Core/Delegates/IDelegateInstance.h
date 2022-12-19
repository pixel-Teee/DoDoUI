#pragma once

#include "Core/Core.h"

#include "Core/Templates/TypeHash.h"

namespace DoDo
{
	/*
	 * class representing a handle to a specific object/function pair bound to a delegate
	 */
	class FDelegateHandle
	{
	public:
		enum EGenerateNewHandleType
		{
			GenerateNewHandle
		};

		//creates an initially unset handle
		FDelegateHandle()
			:m_id(0)
		{
			
		}

		//creates a handle pointing to a new instance
		explicit FDelegateHandle(EGenerateNewHandleType)
			: m_id(Generate_New_Id())
		{
			
		}

		//return true, if this was ever bound to a delegate, but you need to check with the owning delegate to confirm it is still valid
		bool Is_Valid() const
		{
			return m_id != 0;
		}

		//clear handle to indicate it is no longer bound
		void Reset()
		{
			m_id = 0;
		}

	private:
		friend bool operator==(const FDelegateHandle& lhs, const FDelegateHandle& rhs)
		{
			return lhs.m_id == rhs.m_id;
		}

		friend bool operator!=(const FDelegateHandle& lhs, const FDelegateHandle& rhs)
		{
			return lhs.m_id != rhs.m_id;
		}

		friend uint32_t Get_Type_Hash(const FDelegateHandle& key)
		{
			return Get_Type_Hash(key.m_id);
		}
		/*
		 * generates a new id for use the delegate handle
		 *
		 * return a unique id for the delegate
		 */
		static uint64_t Generate_New_Id();

		uint64_t m_id;
	};
}
