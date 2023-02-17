#include <PreCompileHeader.h>

#include "FontCacheFreeType.h"

#include "freetype/ftmodapi.h"

namespace DoDo
{
	FFreeTypeLibrary::FFreeTypeLibrary()
	{
		FT_Error error = FT_Init_FreeType(&m_ftl_library);

		if(error)
		{
			std::cout << "could not init FreeType, error code: " << error << std::endl;
		}

		FT_Add_Default_Modules(m_ftl_library);
	}

	FFreeTypeLibrary::~FFreeTypeLibrary()
	{
	}
}
