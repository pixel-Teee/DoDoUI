#include <PreCompileHeader.h>

#include "CoreStyle.h"

namespace DoDo {
	std::shared_ptr<ISlateStyle> FCoreStyle::m_instance;
	bool FCoreStyle::is_star_ship_style()
	{
		return true;
	}
}