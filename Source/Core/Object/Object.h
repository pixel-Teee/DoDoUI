#pragma once

#include "Core/String/DoDoString.h"

namespace DoDo {
	class Object //todo:?
	{
	public:

		/*returns the logical name of this object*/
		DoDoUtf8String get_name() const {
			return m_name_private;
		}

	private:
		/*name of this object*/
		DoDoUtf8String m_name_private;
	};
}