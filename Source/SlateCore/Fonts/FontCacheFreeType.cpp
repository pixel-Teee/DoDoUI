#include <PreCompileHeader.h>

#include "FontCacheFreeType.h"

#include "freetype/ftmodapi.h"

namespace DoDo
{
	FFreeTypeLibrary::FFreeTypeLibrary()
	{
		FT_Error error = FT_Init_FreeType(&m_ft_library);

		if(error)
		{
			std::cout << "could not init FreeType, error code: " << error << std::endl;
		}

		FT_Add_Default_Modules(m_ft_library);
	}

	FFreeTypeLibrary::~FFreeTypeLibrary()
	{
	}

	namespace FreeTypeUtils
	{
		FT_Error load_glyph(FT_Face in_face, const uint32_t in_glyph_index, const int32_t in_load_flags, const int32_t in_font_size, const float in_font_scale)
		{
			//todo:implement apply size and scale
			return FT_Load_Glyph(in_face, in_glyph_index, in_load_flags);
		}
	}
	
}


